/**
 * Copyright (c)2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.PresenceApp;

import android.annotation.SuppressLint;
import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.ImageView;
import android.widget.TextView;

import androidx.fragment.app.Fragment;

import java.util.ArrayList;
import java.util.List;

/**
 * A simple {@link Fragment} subclass.
 * Use the {@link ContactListFragment#newInstance} factory method to
 * create an instance of this fragment.
 *
 *
 */

public class ContactArrayAdapter<T> extends ArrayAdapter<T> {

    int layoutRes;
    ArrayList<Contact> contacts;

    public ContactArrayAdapter(Context context, int resource,
                               int textViewResourceId, List<T> objects) {
        super(context, resource, textViewResourceId, objects);
        layoutRes = resource;
        contacts = (ArrayList<Contact>) objects;
    }

    @SuppressLint("ResourceAsColor")
    @Override
    public View getView(int position, View convertView, ViewGroup parent) {
        //LinearLayout rowView = convertView;
        LayoutInflater inflater = LayoutInflater.from(getContext());
        convertView =inflater.inflate(layoutRes, parent, false);


        TextView nameView = (TextView) convertView.findViewById(R.id.name);
        TextView phoneView = (TextView) convertView.findViewById(R.id.phone);
        TextView noteView = (TextView) convertView.findViewById(R.id.Note);
        ImageView bmp = (ImageView) convertView.findViewById(R.id.icon);
        ImageView isMultiSelectedIcon = (ImageView) convertView
                .findViewById(R.id.is_selected_icon);
        ImageView isExcludedIcon = (ImageView) convertView
                .findViewById(R.id.is_excluded_icon);
        if(contacts != null && contacts.get(position) != null) {

            nameView.setText(contacts.get(position).getName());
            phoneView.setText(contacts.get(position).getPhone());
            //update with VoLTE/VT status
            noteView.setText("<Not Subscribed>");
            if(contacts.get(position).isSubscribed()){
                noteView.setText("Subscribed");
                noteView.setTextColor(R.color.SubAvailable);
            }

        }
        updateIcon(bmp, position);
        updateExcludedIcon(isExcludedIcon, false);
//        if (contacts != null && contacts.get(position) != null) {
//            updateExcludedIcon(isExcludedIcon, true);
//            updateMultiSelectedIcon(isMultiSelectedIcon, false);
//
//        }
        return convertView;
    }

    private void updateExcludedIcon(ImageView isExcludedIcon,
                                    boolean visible) {
        if (visible) {
            isExcludedIcon.setVisibility(View.VISIBLE);
        } else {
            isExcludedIcon.setVisibility(View.INVISIBLE);
        }
    }

    private void updateMultiSelectedIcon(ImageView icon,
                                         boolean multiSelected) {
        if (multiSelected) {
            icon.setVisibility(View.VISIBLE);
        } else {
            icon.setVisibility(View.INVISIBLE);
        }
    }

    private void updateIcon(ImageView bmp, int position) {
//        switch (contacts.get(position).getAvailability()) {
//            case 0:
//                bmp.setImageResource(getResources().getIdentifier("icon",
//                        "drawable", getPackageName()));
//                break;
//
//            case 1:
//                bmp.setImageResource(getResources().getIdentifier("online",
//                        "drawable", getPackageName()));
//                break;
//
//            case 2:
//                bmp.setImageResource(getResources().getIdentifier("busy",
//                        "drawable", getPackageName()));
//                break;
//
//            default:
//                bmp.setImageResource(getResources().getIdentifier("icon",
//                        "drawable", getPackageName()));
//        }
    }
}
