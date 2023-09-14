/* Automatically generated nanopb constant definitions */
/* Generated by nanopb-0.3.9.5 at Fri Jun  5 11:03:58 2020. */

#include "qsh_location.pb.h"

/* @@protoc_insertion_point(includes) */
#if PB_PROTO_HEADER_VERSION != 30
#error Regenerate this file with the current version of nanopb generator.
#endif



const pb_field_t qsh_location_open_fields[2] = {
    PB_FIELD(  1, UINT32  , REQUIRED, STATIC  , FIRST, qsh_location_open, version, version, 0),
    PB_LAST_FIELD
};

const pb_field_t qsh_location_update_fields[4] = {
    PB_FIELD(  1, BOOL    , REQUIRED, STATIC  , FIRST, qsh_location_update, start, start, 0),
    PB_FIELD(  2, UENUM   , REQUIRED, STATIC  , OTHER, qsh_location_update, location_request, start, 0),
    PB_FIELD(  3, UINT32  , REQUIRED, STATIC  , OTHER, qsh_location_update, interval, location_request, 0),
    PB_LAST_FIELD
};

const pb_field_t qsh_location_ack_fields[5] = {
    PB_FIELD(  1, BOOL    , REQUIRED, STATIC  , FIRST, qsh_location_ack, error, error, 0),
    PB_FIELD(  2, UENUM   , REQUIRED, STATIC  , OTHER, qsh_location_ack, command, error, 0),
    PB_FIELD(  3, UINT32  , OPTIONAL, STATIC  , OTHER, qsh_location_ack, version, command, 0),
    PB_FIELD(  4, UINT32  , OPTIONAL, STATIC  , OTHER, qsh_location_ack, capabilities, version, 0),
    PB_LAST_FIELD
};

const pb_field_t qsh_location_position_event_fields[12] = {
    PB_FIELD(  1, UINT64  , REQUIRED, STATIC  , FIRST, qsh_location_position_event, timestamp, timestamp, 0),
    PB_FIELD(  2, INT32   , OPTIONAL, STATIC  , OTHER, qsh_location_position_event, latitude, timestamp, 0),
    PB_FIELD(  3, INT32   , OPTIONAL, STATIC  , OTHER, qsh_location_position_event, longitude, latitude, 0),
    PB_FIELD(  4, FLOAT   , OPTIONAL, STATIC  , OTHER, qsh_location_position_event, altitude, longitude, 0),
    PB_FIELD(  5, FLOAT   , OPTIONAL, STATIC  , OTHER, qsh_location_position_event, horizontal_accuracy, altitude, 0),
    PB_FIELD(  6, FLOAT   , OPTIONAL, STATIC  , OTHER, qsh_location_position_event, vertical_accuracy, horizontal_accuracy, 0),
    PB_FIELD(  7, FLOAT   , OPTIONAL, STATIC  , OTHER, qsh_location_position_event, speed, vertical_accuracy, 0),
    PB_FIELD(  8, FLOAT   , OPTIONAL, STATIC  , OTHER, qsh_location_position_event, speed_accuracy, speed, 0),
    PB_FIELD(  9, FLOAT   , OPTIONAL, STATIC  , OTHER, qsh_location_position_event, bearing, speed_accuracy, 0),
    PB_FIELD( 10, FLOAT   , OPTIONAL, STATIC  , OTHER, qsh_location_position_event, bearing_accuracy, bearing, 0),
    PB_FIELD( 11, FLOAT   , OPTIONAL, STATIC  , OTHER, qsh_location_position_event, conformity_index, bearing_accuracy, 0),
    PB_LAST_FIELD
};

const pb_field_t qsh_location_measurement_fields[16] = {
    PB_FIELD(  1, INT32   , REQUIRED, STATIC  , FIRST, qsh_location_measurement, sv_id, sv_id, 0),
    PB_FIELD(  2, UENUM   , REQUIRED, STATIC  , OTHER, qsh_location_measurement, constellation_type, sv_id, 0),
    PB_FIELD(  3, INT64   , REQUIRED, STATIC  , OTHER, qsh_location_measurement, time_offset, constellation_type, 0),
    PB_FIELD(  4, UINT32  , REQUIRED, STATIC  , OTHER, qsh_location_measurement, state, time_offset, 0),
    PB_FIELD(  5, INT64   , REQUIRED, STATIC  , OTHER, qsh_location_measurement, received_sv_time, state, 0),
    PB_FIELD(  6, INT64   , REQUIRED, STATIC  , OTHER, qsh_location_measurement, received_sv_time_uncertainty, received_sv_time, 0),
    PB_FIELD(  7, FLOAT   , REQUIRED, STATIC  , OTHER, qsh_location_measurement, c_n0, received_sv_time_uncertainty, 0),
    PB_FIELD(  8, FLOAT   , REQUIRED, STATIC  , OTHER, qsh_location_measurement, pseudorange_rate, c_n0, 0),
    PB_FIELD(  9, FLOAT   , REQUIRED, STATIC  , OTHER, qsh_location_measurement, pseudorange_rate_uncertainty, pseudorange_rate, 0),
    PB_FIELD( 10, DOUBLE  , OPTIONAL, STATIC  , OTHER, qsh_location_measurement, carrier_phase, pseudorange_rate_uncertainty, 0),
    PB_FIELD( 11, FLOAT   , OPTIONAL, STATIC  , OTHER, qsh_location_measurement, carrier_phase_uncertainty, carrier_phase, 0),
    PB_FIELD( 12, UINT32  , OPTIONAL, STATIC  , OTHER, qsh_location_measurement, cycle_slip_count, carrier_phase_uncertainty, 0),
    PB_FIELD( 13, UENUM   , OPTIONAL, STATIC  , OTHER, qsh_location_measurement, multipath_indicator, cycle_slip_count, 0),
    PB_FIELD( 14, FLOAT   , OPTIONAL, STATIC  , OTHER, qsh_location_measurement, snr, multipath_indicator, 0),
    PB_FIELD( 15, FLOAT   , OPTIONAL, STATIC  , OTHER, qsh_location_measurement, carrier_frequency, snr, 0),
    PB_LAST_FIELD
};

const pb_field_t qsh_location_clock_fields[9] = {
    PB_FIELD(  1, INT64   , REQUIRED, STATIC  , FIRST, qsh_location_clock, time, time, 0),
    PB_FIELD(  2, FLOAT   , OPTIONAL, STATIC  , OTHER, qsh_location_clock, time_uncertainty, time, 0),
    PB_FIELD(  3, INT64   , OPTIONAL, STATIC  , OTHER, qsh_location_clock, full_bias, time_uncertainty, 0),
    PB_FIELD(  4, FLOAT   , OPTIONAL, STATIC  , OTHER, qsh_location_clock, bias, full_bias, 0),
    PB_FIELD(  5, FLOAT   , OPTIONAL, STATIC  , OTHER, qsh_location_clock, bias_uncertainty, bias, 0),
    PB_FIELD(  6, FLOAT   , OPTIONAL, STATIC  , OTHER, qsh_location_clock, drift, bias_uncertainty, 0),
    PB_FIELD(  7, FLOAT   , OPTIONAL, STATIC  , OTHER, qsh_location_clock, drift_uncertainty, drift, 0),
    PB_FIELD(  8, UINT32  , OPTIONAL, STATIC  , OTHER, qsh_location_clock, hw_clock_discontinuity_count, drift_uncertainty, 0),
    PB_LAST_FIELD
};

const pb_field_t qsh_location_meas_and_clk_event_fields[5] = {
    PB_FIELD(  1, UINT32  , REQUIRED, STATIC  , FIRST, qsh_location_meas_and_clk_event, number_of_measurements, number_of_measurements, 0),
    PB_FIELD(  2, MESSAGE , REPEATED, CALLBACK, OTHER, qsh_location_meas_and_clk_event, measurements, number_of_measurements, &qsh_location_measurement_fields),
    PB_FIELD(  3, MESSAGE , REQUIRED, STATIC  , OTHER, qsh_location_meas_and_clk_event, clock, measurements, &qsh_location_clock_fields),
    PB_FIELD(  4, UINT32  , REQUIRED, STATIC  , OTHER, qsh_location_meas_and_clk_event, measurement_index, clock, 0),
    PB_LAST_FIELD
};








/* Check that field information fits in pb_field_t */
#if !defined(PB_FIELD_32BIT)
/* If you get an error here, it means that you need to define PB_FIELD_32BIT
 * compile-time option. You can do that in pb.h or on compiler command line.
 *
 * The reason you need to do this is that some of your messages contain tag
 * numbers or field sizes that are larger than what can fit in 8 or 16 bit
 * field descriptors.
 */
PB_STATIC_ASSERT((pb_membersize(qsh_location_meas_and_clk_event, clock) < 65536), YOU_MUST_DEFINE_PB_FIELD_32BIT_FOR_MESSAGES_qsh_location_open_qsh_location_update_qsh_location_ack_qsh_location_position_event_qsh_location_measurement_qsh_location_clock_qsh_location_meas_and_clk_event)
#endif

#if !defined(PB_FIELD_16BIT) && !defined(PB_FIELD_32BIT)
/* If you get an error here, it means that you need to define PB_FIELD_16BIT
 * compile-time option. You can do that in pb.h or on compiler command line.
 *
 * The reason you need to do this is that some of your messages contain tag
 * numbers or field sizes that are larger than what can fit in the default
 * 8 bit descriptors.
 */
PB_STATIC_ASSERT((pb_membersize(qsh_location_meas_and_clk_event, clock) < 256), YOU_MUST_DEFINE_PB_FIELD_16BIT_FOR_MESSAGES_qsh_location_open_qsh_location_update_qsh_location_ack_qsh_location_position_event_qsh_location_measurement_qsh_location_clock_qsh_location_meas_and_clk_event)
#endif


/* On some platforms (such as AVR), double is really float.
 * These are not directly supported by nanopb, but see example_avr_double.
 * To get rid of this error, remove any double fields from your .proto.
 */
PB_STATIC_ASSERT(sizeof(double) == 8, DOUBLE_MUST_BE_8_BYTES)

/* @@protoc_insertion_point(eof) */