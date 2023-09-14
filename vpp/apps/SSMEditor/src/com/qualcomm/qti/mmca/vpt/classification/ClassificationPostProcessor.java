/**
 * Copyright (c) 2019-2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * @file ClassificationPostProcessor.java
 */
package com.qualcomm.qti.mmca.vpt.classification;

import android.util.Log;

import com.qualcomm.qti.mmca.ssmeditor.FRCSegment;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

public class ClassificationPostProcessor {

    private static class ClassificationDefinition {
        public final static int THRESHOLD_MAX = 1;

        public String id;
        public String label;
        public double threshold;
        public int duration;

        public ClassificationDefinition(String id, String label, double threshold, int duration) {
            this.id = id;
            this.label = label;
            this.threshold = threshold;
            this.duration = duration;
        }
    }

    private static class Classification extends ClassificationDefinition {
        double logit;
        double probability;

        public Classification(String id, String label, double threshold) {
            this(id, label, threshold, 0);
        }

        public Classification(String id, String label, double threshold, int duration) {
            super(id, label, threshold, duration);
        }

        public Classification(ClassificationDefinition other) {
            this(other.id, other.label, other.threshold, other.duration);
        }
    }

    private static String TAG = "PostProc";

    private static HashMap<String, ClassificationDefinition> CLASS_CONFIG = new HashMap();
    private static int WINDOW_SIZE = 3; // must be odd
    private static int WINDOW_CENTER_INDEX = WINDOW_SIZE / 2;
    private static int FRAMES_INCREMENT = 1; // overlap

    static {
        addToConfigMap(new ClassificationDefinition("1", "INTERESTING",     0.55, 2));
        addToConfigMap(new ClassificationDefinition("0", "NOT INTERESTING", 1.00, 0));
    }

    private static void addToConfigMap(ClassificationDefinition definition) {
        CLASS_CONFIG.put(definition.id, definition);
    }

    public ClassificationPostProcessor() {
        Log.d(TAG, "window size: " + WINDOW_SIZE + ", center index=" + WINDOW_CENTER_INDEX);
    }

    public List<VPTSegment> execute(Metadata metadata) {
        List<ClassificationBatch> batches = metadata.getBatches();
        int batchesToProcess = batches.size();

        int processedCount = 0;

        Log.d(TAG, "batchesToProcess=" + batchesToProcess);

        int framesToCapture = -1;

        List<VPTSegment> outputSegments = new ArrayList<>();
        while (batchesToProcess >= WINDOW_SIZE) {

            int startIndex = processedCount;
            int endIndex = startIndex + WINDOW_SIZE;
            List<ClassificationBatch> currentWindow = batches.subList(startIndex, endIndex);

            // bottom path
            List<InterestingPeakResult> interestingPeaks = detectInterestingPeaks(currentWindow);

            // top path
            ClassificationBatch center = currentWindow.get(currentWindow.size() / 2);
            List<Classification> classifications = convertLogitsToProbabilities(center);
            Classification selectedClass = determineInterestingClass(classifications);

            // final decision
            String selectedLabel = "";
            int selectedIndex = -1;
            double selectedProbability = 0.;
            double selectedThreshold = ClassificationDefinition.THRESHOLD_MAX;

            if (selectedClass != null) {
                for (InterestingPeakResult ipr : interestingPeaks) {
                    if (ipr.classification.id == selectedClass.id) {
                        selectedIndex = ipr.windowIndex;
                        selectedLabel = ipr.classification.label;
                        break;
                    }
                }

                selectedProbability = selectedClass.probability;
                selectedThreshold = selectedClass.threshold;

                if (selectedIndex >= WINDOW_CENTER_INDEX || framesToCapture == 0) {
                    framesToCapture = selectedClass.duration;
                }
            } else {
                selectedProbability = getDefaultProbability(classifications);
            }

            long startTimeUs = center.getStartTime() / 1000;
            long endTimeUs = center.getEndTime() / 1000;

            VPTSegment s = new VPTSegment(startTimeUs, endTimeUs, FRCSegment.Interp.BYPASS,
                    selectedLabel, (int)(selectedProbability * 100));
            if (framesToCapture >= 0) {
                framesToCapture -= FRAMES_INCREMENT;
                s.setInterp(FRCSegment.Interp.INTERP_2X);
            }

            // only add segments generated from valid frames
            if (center.containsAnyValidFrames()) {
                outputSegments.add(s);
            }

            Log.d(TAG, "execute(" + processedCount + ", " + endIndex + ") " +
                    String.format("%15s", selectedLabel) + ": " +
                    "selectedWindowIndex=" + selectedIndex +
                    ", currentTs=[" + startTimeUs +
                    ", " + endTimeUs +
                    "], threshold=" + selectedThreshold +
                    ", prob=" + String.format("%.2f", selectedProbability) +
                    ", interp=" + s.getInterp() +
                    ", framesToCapture=" + framesToCapture +
                    ", numOutputSegments=" + outputSegments.size());

            processedCount++;
            batchesToProcess -= 1;
        }

        Log.d(TAG, "batchesToProcess=" + batchesToProcess);

        return outputSegments;
    }

    private class InterestingPeakResult {
        int windowIndex;
        ClassificationDefinition classification;
    }

    private List<InterestingPeakResult> detectInterestingPeaks(List<ClassificationBatch> window) {

        int numClasses = CLASS_CONFIG.size();
        int windowSize = window.size();

        // Table of logits for classes which are defined in our CLASS_CONFIG
        // for the current window over which this function is operating.
        double logits[][] = new double[numClasses][windowSize];

        // Mapping of the string label to the position in the |logits| table.
        HashMap<String, Integer> labelToIndex = new HashMap<>();

        // Mapping of the position in |logits| back to the class definitions.
        HashMap<Integer, ClassificationDefinition> indexToClassDef = new HashMap<>();

        // Fill out labelToIndex and indexToClassDef at the same time that we
        // are populating the logits table.
        for (int i = 0; i < window.size(); i++) {
            ClassificationBatch batch = window.get(i);
            List<ClassificationResult> results = batch.getClassificationResults();
            for (ClassificationResult result : results) {
                String id = result.getClassId();
                // We only care about logits that we have definitions for.
                if (CLASS_CONFIG.containsKey(id)) {
                    if (!labelToIndex.containsKey(id)) {
                        int index = labelToIndex.size();
                        labelToIndex.put(id, index);
                        indexToClassDef.put(index, CLASS_CONFIG.get(id));
                    }

                    logits[labelToIndex.get(id)][i] = result.getLogit();
                }
            }
        }

        // Compute diffs of logits for each classification between adjacent
        // batches, and determine the batch with highest delta for each
        // classification.
        Integer[] maxDeltaIndex = new Integer[numClasses];
        Double[] maxDeltaValue = new Double[numClasses];
        for (int i = 0; i < maxDeltaIndex.length; i++) {
            maxDeltaIndex[i] = -1;
            maxDeltaValue[i] = 0.;
        }
        double logitDiffs[][] = new double[numClasses][windowSize - 1];
        for (int classIndex = 0; classIndex < numClasses; classIndex++) {
            for (int windowIndex = 1; windowIndex < windowSize; windowIndex++) {

                double previous = logits[classIndex][windowIndex - 1];
                double current = logits[classIndex][windowIndex];
                double diff = current - previous;

                logitDiffs[classIndex][windowIndex - 1] = diff;
                if (maxDeltaIndex[classIndex] == -1 ||
                        maxDeltaValue[classIndex] < diff) {
                    maxDeltaIndex[classIndex] = windowIndex;
                    maxDeltaValue[classIndex] = diff;
                }
            }
        }

        // translate class index back to class name
        List<InterestingPeakResult> peaks = new ArrayList<>(numClasses);
        for (int classIndex = 0; classIndex < numClasses; classIndex++) {
            InterestingPeakResult peakResult = new InterestingPeakResult();
            peakResult.classification = indexToClassDef.get(classIndex);
            peakResult.windowIndex = maxDeltaIndex[classIndex];
            peaks.add(peakResult);
        }

        // DEBUG
        for (int i = 0; i < logits.length; i++) { // class
            // Debug tables into following format:
            // CLASS: logit0, logit1, ..., logitN
            StringBuilder sb = new StringBuilder();
            sb.append(String.format("%15s: logits:", indexToClassDef.get(i).label));
            for (int j = 0; j < logits[i].length; j++) { // logits
                sb.append(String.format("  % 2.2f", logits[i][j]));
            }

            sb.append(" | deltas:");
            for (int j = 0; j < logitDiffs[i].length; j++) {
                sb.append(String.format("  % 2.2f", logitDiffs[i][j]));
            }

            sb.append(" | maxDeltaIndex:");
            sb.append(String.format(" %d", maxDeltaIndex[i]));
            Log.d(TAG, sb.toString());
        }

        return peaks;
    }

    private List<Classification> convertLogitsToProbabilities(ClassificationBatch r) {
        List<ClassificationResult> classificationResults = r.getClassificationResults();

        // convert logits to probabilities
        List<Double> logits = new ArrayList<>(classificationResults.size());
        classificationResults.forEach(c -> logits.add((double)c.getLogit()));
        List<Double> probabilities = softmax(logits);

        // Create Classification objects for each result. For classes which
        // have not been defined, we still need to map it to something, so
        // create a dummy for it, with threshold set to max. This will
        // guarantee that it won't be selected as the probability can not
        // exceed the max.
        List<Classification> classifications = new ArrayList<>();
        for (int i = 0; i < classificationResults.size(); i++) {
            double probability = probabilities.get(i);
            double logit = logits.get(i);
            ClassificationResult cls = classificationResults.get(i);
            String id = cls.getClassId();

            Classification c;
            if (CLASS_CONFIG.containsKey(id)) {
                ClassificationDefinition def = CLASS_CONFIG.get(id);
                c = new Classification(def);
                c.probability = probability;
                c.logit = logit;
            } else {
                c = new Classification(id, "UNKNOWN", ClassificationDefinition.THRESHOLD_MAX);
                c.probability = probability;
                c.logit = logit;
            }
            classifications.add(c);
        }

        return classifications;
    }

    private double getDefaultProbability(List<Classification> classes) {
        // we didn't select a class but we probably want to give some sort of
        // probability to the client... since we're only binary (interesting
        // or not), we can give the probability of interesting...
        for (Classification c : classes) {
            if (c.label == "INTERESTING") {
                return c.probability;
            }
        }
        return 0;
    }

    private Classification determineInterestingClass(List<Classification> classes) {
        Classification selectedClass = null;
        double maxExceedingThreshold = -1;

        StringBuilder sb = new StringBuilder();
        sb.append("SelectClass: ");

        for (Classification c : classes) {
            sb.append(String.format("%s [p=%.2f, t=%.2f", c.label, c.probability, c.threshold));
            if (c.probability > c.threshold) {
                double amountExceedingThreshold = c.probability - c.threshold;
                sb.append(String.format(" d=%.2f", amountExceedingThreshold));
                if (amountExceedingThreshold > maxExceedingThreshold) {
                    maxExceedingThreshold = amountExceedingThreshold;
                    selectedClass = c;
                }
            }
            sb.append("] ");
        }

        sb.append(" selected=" + (selectedClass == null ? "null" : selectedClass.label));
        Log.d(TAG, sb.toString());

        return selectedClass;
    }

    private List<Double> softmax(List<Double> input) {
        double sumExp = 0;
        List<Double> exp = new ArrayList<>(input.size());
        List<Double> output = new ArrayList<>(input.size());

        input.forEach(i -> exp.add(Math.exp(i)));

        for (Double e : exp) {
            sumExp += e;
        }

        final double sum = sumExp;
        exp.forEach(i -> output.add(i / sum));

        // DEBUG
        // TODO: this is easily unit testable
        double sumProb = 0.;
        for (Double i : output) {
            sumProb += i;
        }

        StringBuilder sb = new StringBuilder();
        sb.append("softmax: sumProb=" + String.format("%.2f", sumProb) + ", in=[");
        input.forEach(i -> sb.append(String.format("%.2f", i) + ", "));
        sb.delete(sb.length() - 2, sb.length() - 1);

        sb.append("], out=[");
        output.forEach(i -> sb.append(String.format("%.2f", i) + ", "));
        sb.delete(sb.length() - 2, sb.length() - 1);
        sb.append("]");

        Log.d(TAG, sb.toString());

        return output;
    }
}
