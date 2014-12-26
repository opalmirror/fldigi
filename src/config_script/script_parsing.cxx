/** **************************************************************
 \page script_parsing Script Parsing Class

 \par script_parsing.cxx (FLDIGI)

 \par Author(s):
 Robert Stiles, KK5VD, Copyright &copy; 2014
 <br>
 <br>
 This is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 3 of the License, or
 (at your option) any later version. This software is distributed in
 the hope that it will be useful, but WITHOUT ANY WARRANTY; without
 even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the GNU General Public License for more details. You
 should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 <br>
 <br>

 \par USE:
 Create a structure of data contained within the SCRIPT_COMMANDS
 structure \(see \ref script_cmds\).  Each element are as follows.<br>

 \verbatim
 { SCRIPT_COMMAND, 0, "AUTO LOAD QUEUE",  1, {0}, { p_string }, 0, 0, 0, 0 }
 \endverbatim

 \par "int flags <SCRIPT_COMMAND and/or QUEUE_COMMAND>"
 Indicates if the script command is a global assignment (SCRIPT_COMMAND)
 or local for each of the queued files (QUEUE_COMMAND).

 \par "size_t command_length"
 Programmer assignment not required. Internally Assigned.  This is the
 length of the script command in bytes.

 \par "char command[MAX_COMMAND_LENGTH]"
 The script command used to match within the content of the script
 file. MAX_COMMAND_LENGTH is the maximum length of the supplied buffer.

 \par "int  argc"
 The number of argument as required for the script command. The
 parsing engine will limit the number of scanned parameters even of more
 has been supplied.

 \par "char *args[MAX_CMD_PARAMETERS]"
 An array of char string pointers to each parameter scanned for
 the specific command. MAX_CMD_PARAMETERS is the maximum number
 of positions available in the parameter buffer table.

 \par "enum paramter_types param_check[MAX_CMD_PARAMETERS]"
 A list of validation flags for each parameters required.
 MAX_CMD_PARAMETERS is the maximum number of positions available
 in the param_check buffer table. See \ref paramter_types for a list
 of valid values.<br>

 \par "calling_func func"
 When a command string is matched in the script file, this StringParsing
 Class member is executed for further processing of the command
 paramaters. This can include any validation checks if required. The member
 functions are assigned during the creation of the class instance. See the
 constructor member ScriptParsing::ScriptParsing() for details.

 \par "const char **valid_values"
 List of valid paramters. Use function
 int assign_valid_parameters(const char *command, const char **array, const int array_count);
 to asssign the values to a specific script command.

 \par "int valid_value_count"
 Number of valid paramters.

 \par "int (*cb)(ScriptParsing *sp, struct script_cmds *sd)"
 This is assigned using the StringParsing Member:<br>
 <br>
 int assign_callback(const char *scriptCommand, int (*cb)(ScriptParsing *sp, SCRIPT_COMMANDS *sc));<br>
 <br>
 The function which is supplied by the programmer after the creation of the
 Class instance is called when the command is matched. This allows the
 programmer access to a multitude of information for further processing
 outside of the ScriptParsing Class instance.<br>
 <br>
 Example:<br>
 \verbatim
 #include "script_parsing.h"

 static const char *modems[] = {
 "BPSK31",
 "MFSK32",
 "MT63-500",
 "MFSK64"
 };

 int callback(ScriptParsing *sp, struct script_cmds *sc)
 {
 // do something
 return 0;
 }

 int main(int argc, const char * argv[])
 {
 ScriptParsing *sp = new ScriptParsing;

 if(sp) {
 sp->assign_valid_parameters("MODEM", modems, sizeof(modems)/sizeof(char *));
 sp->assign_callback("FILE", callback);
 sp->parse_commands((char *)"/fldigi-dev/test_parse_commands/running_test.txt");
 }

 return 0;
 }
 \endverbatim
 <br>
 See \ref script_parsing_class and \ref script_cmds for details about
 what data is provided by the ScriptParsing *sp and SCRIPT_COMMANDS *sc
 pointers. The passed SCRIPT_COMMANDS *sc pointer is a copy of the
 original data. Modification of this information does not alter the
 internal data.<br><br>
 <b>Note:</b> The member and function pointers within the SCRIPT_COMMANDS
 *sc pointer are set to dummy functions which return back to the caller
 if executed.
 *******************************************************************/

#include "config.h"
#include "util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/stat.h>

//#define EXTERNAL_TESTING
#undef EXTERNAL_TESTING
#ifdef EXTERNAL_TESTING
#define TESTING 1
#define LOG_INFO printf
#else
#include "debug.h"
#endif

#include "gettext.h"
#include "script_parsing.h"
#include "run_script.h"

// These tables (by reference) are not used. Copy to another memory location.

static const SCRIPT_COMMANDS default_operator_command_table[] = {
	{ CMD_CALLSIGN,                SCRIPT_COMMAND, 0,  1, {0}, { p_string   }, 0, 0, 0, process_callsign_info, 0, 0},
	{ CMD_NAME,                    SCRIPT_COMMAND, 0,  1, {0}, { p_string   }, 0, 0, 0, process_name_info,     0, 0},
	{ CMD_QTH,                     SCRIPT_COMMAND, 0,  1, {0}, { p_string   }, 0, 0, 0, process_qth_info,      0, 0},
	{ CMD_LOCATOR,                 SCRIPT_COMMAND, 0,  1, {0}, { p_string   }, 0, 0, 0, process_locator_info,  0, 0},
	{ CMD_ANTENNA,                 SCRIPT_COMMAND, 0,  1, {0}, { p_string   }, 0, 0, 0, process_antenna_info,  0, 0},
	{ CMD_OPERATOR_END,            SCRIPT_COMMAND, 0,  0, {0}, { p_list_end }, 0, 0, 0, 0, 0, 0},
	{ {0} }
};

static const SCRIPT_COMMANDS default_audio_device_command_table[] = {
	{ CMD_OSS_AUDIO,               SCRIPT_COMMAND, 0,  1, {0}, { p_bool     }, 0, 0, 0, process_use_oss_audio_device,    0, 0},
	{ CMD_OSS_AUDIO_DEV_PATH,      SCRIPT_COMMAND, 0,  1, {0}, { p_string   }, 0, 0, 0, process_oss_audio_device_path,   0, 0},
	{ CMD_PORT_AUDIO,              SCRIPT_COMMAND, 0,  1, {0}, { p_bool     }, 0, 0, 0, process_use_port_audio_device,   0, 0},
	{ CMD_PORTA_CAP,               SCRIPT_COMMAND, 0,  1, {0}, { p_dev_name }, 0, 0, 0, process_capture_path,            0, 0},
	{ CMD_PORTA_PLAY,              SCRIPT_COMMAND, 0,  1, {0}, { p_dev_name }, 0, 0, 0, process_playback_path,           0, 0},
	{ CMD_PULSEA,                  SCRIPT_COMMAND, 0,  1, {0}, { p_bool     }, 0, 0, 0, process_use_pulse_audio_device,  0, 0},
	{ CMD_PULSEA_SERVER,           SCRIPT_COMMAND, 0,  1, {0}, { p_dev_path }, 0, 0, 0, process_pulse_audio_device_path, 0, 0},
	{ CMD_AUDIO_DEVICE_END,        SCRIPT_COMMAND, 0,  0, {0}, { p_list_end }, 0, 0, 0, 0, 0, 0},
	{ {0} }
};

static const SCRIPT_COMMANDS default_audio_settings_command_table[] = {
	{ CMD_CAPTURE_SAMPLE_RATE,     SCRIPT_COMMAND, 0,  1, {0}, { p_string   },     0, 0, 0, process_audio_device_sample_rate_capture, 0, 0},
	{ CMD_PLAYBACK_SAMPLE_RATE,    SCRIPT_COMMAND, 0,  1, {0}, { p_string   },     0, 0, 0, process_audio_device_sample_rate_playback, 0, 0},
	{ CMD_AUDIO_CONVERTER,         SCRIPT_COMMAND, 0,  1, {0}, { p_string   },     0, 0, 0, process_audio_device_converter, 0, 0},
	{ CMD_RX_PPM,                  SCRIPT_COMMAND, 0,  1, {0}, { p_unsigned_int }, 0, 0, 0, process_rx_ppm, 0, 0},
	{ CMD_TX_PPM,                  SCRIPT_COMMAND, 0,  1, {0}, { p_unsigned_int }, 0, 0, 0, process_tx_ppm, 0, 0},
	{ CMD_TX_OFFSET,               SCRIPT_COMMAND, 0,  1, {0}, { p_unsigned_int }, 0, 0, 0, process_tx_offset, 0, 0},
	{ CMD_AUDIO_SETTINGS_END,      SCRIPT_COMMAND, 0,  0, {0}, { p_list_end }, 0, 0, 0, 0, 0, 0},
	{ {0} }
};

static const SCRIPT_COMMANDS default_audio_rt_channel_command_table[] = {
	{ CMD_MONO_AUDIO,              SCRIPT_COMMAND, 0,  1, {0}, { p_bool }, 0, 0, 0, process_mono_audio_output,        0, 0},
	{ CMD_AUDIO_L_R,               SCRIPT_COMMAND, 0,  1, {0}, { p_bool }, 0, 0, 0, process_modem_signal_left_right,  0, 0},
	{ CMD_AUDIO_REV_L_R,           SCRIPT_COMMAND, 0,  1, {0}, { p_bool }, 0, 0, 0, process_reverse_left_right,       0, 0},
	{ CMD_PTT_RIGHT_CHAN,          SCRIPT_COMMAND, 0,  1, {0}, { p_bool }, 0, 0, 0, process_ptt_tone_right_channel,   0, 0},
	{ CMD_CW_QSK_RT_CHAN,          SCRIPT_COMMAND, 0,  1, {0}, { p_bool }, 0, 0, 0, process_cw_qsk_right_channel,     0, 0},
	{ CMD_PSEUDO_FSK_RT_CHAN,      SCRIPT_COMMAND, 0,  1, {0}, { p_bool }, 0, 0, 0, process_pseudo_fsk_right_channel, 0, 0},
	{ CMD_AUDIO_RT_CHANNEL_END,    SCRIPT_COMMAND, 0,  0, {0}, { p_list_end }, 0, 0, 0, 0, 0, 0},
	{ {0} }
};

static const SCRIPT_COMMANDS default_rig_hrdwr_ptt_command_table[] = {
	{ CMD_HPPT_PTT_RT,             SCRIPT_COMMAND, 0,  1, {0}, { p_bool },         0, 0, 0, process_hrdw_ptt_right_audio_channel, 0, 0},
	{ CMD_HPTT_SP2,                SCRIPT_COMMAND, 0,  1, {0}, { p_bool },         0, 0, 0, process_hrdw_ptt_sep_serial_port, 0, 0},
	{ CMD_HPTT_SP2_PATH,           SCRIPT_COMMAND, 0,  1, {0}, { p_dev_path },     0, 0, 0, process_hrdw_ptt_sep_serial_port_path, 0, 0},
	{ CMD_HPTT_SP2_RTS,            SCRIPT_COMMAND, 0,  1, {0}, { p_bool },         0, 0, 0, process_hrdw_ptt_sep_serial_port_rts, 0, 0},
	{ CMD_HPTT_SP2_DTR,            SCRIPT_COMMAND, 0,  1, {0}, { p_bool },         0, 0, 0, process_hrdw_ptt_sep_serial_port_dtr, 0, 0},
	{ CMD_HPTT_SP2_RTS_V,          SCRIPT_COMMAND, 0,  1, {0}, { p_bool },         0, 0, 0, process_hrdw_ptt_sep_serial_port_rts_v, 0, 0},
	{ CMD_HPTT_SP2_DTR_V,          SCRIPT_COMMAND, 0,  1, {0}, { p_bool },         0, 0, 0, process_hrdw_ptt_sep_serial_port_dtr_v, 0, 0},
	{ CMD_HPTT_SP2_START_DELAY,    SCRIPT_COMMAND, 0,  1, {0}, { p_unsigned_int }, 0, 0, 0, process_hrdw_ptt_start_delay, 0, 0},
	{ CMD_HPTT_SP2_END_DELAY,      SCRIPT_COMMAND, 0,  1, {0}, { p_unsigned_int }, 0, 0, 0, process_hrdw_ptt_end_delay, 0, 0},
	{ CMD_HPTT_UHROUTER,           SCRIPT_COMMAND, 0,  1, {0}, { p_bool },         0, 0, 0, process_hrdw_ptt_uhrouter, 0, 0},
	{ CMD_HPTT_SP2_INITIALIZE,     SCRIPT_COMMAND, 0,  1, {0}, { p_void },         0, 0, 0, process_hrdw_ptt_initialize, 0, 0},
	{ CMD_HRDWR_PTT_END,           SCRIPT_COMMAND, 0,  0, {0}, { p_list_end }, 0, 0, 0, 0, 0, 0},
	{ {0} }
};


static const SCRIPT_COMMANDS default_rigcat_command_table[] = {
	{ CMD_RIGCAT_STATE,            SCRIPT_COMMAND, 0,  1, {0}, { p_bool },         0, 0, 0, process_use_rigcat, 0, 0},
	{ CMD_RIGCAT_DESC_FILE,        SCRIPT_COMMAND, 0,  1, {0}, { p_filename },     0, 0, 0, process_rigcat_desc_file, 0, 0},
	{ CMD_RIGCAT_DEV_PATH,         SCRIPT_COMMAND, 0,  1, {0}, { p_dev_path },     0, 0, 0, process_rigcat_device_path, 0, 0},
	{ CMD_RIGCAT_RETRIES,          SCRIPT_COMMAND, 0,  1, {0}, { p_unsigned_int }, 0, 0, 0, process_rigcat_retries, 0, 0},
	{ CMD_RIGCAT_RETRY_INTERVAL,   SCRIPT_COMMAND, 0,  1, {0}, { p_unsigned_int }, 0, 0, 0, process_rigcat_retry_interval, 0, 0},
	{ CMD_RIGCAT_WRITE_DELAY,      SCRIPT_COMMAND, 0,  1, {0}, { p_unsigned_int }, 0, 0, 0, process_rigcat_write_delay, 0, 0},
	{ CMD_RIGCAT_INTIAL_DELAY,     SCRIPT_COMMAND, 0,  1, {0}, { p_unsigned_int }, 0, 0, 0, process_rigcat_init_delay, 0, 0},
	{ CMD_RIGCAT_BAUD_RATE,        SCRIPT_COMMAND, 0,  1, {0}, { p_string },       0, 0, 0, process_rigcat_baud_rate, 0, 0},
	{ CMD_RIGCAT_STOP_BITS,        SCRIPT_COMMAND, 0,  1, {0}, { p_float  },       0, 0, 0, process_rigcat_stop_bits, 0, 0},
	{ CMD_RIGCAT_ECHO,             SCRIPT_COMMAND, 0,  1, {0}, { p_bool },         0, 0, 0, process_rigcat_commands_echoed, 0, 0},
	{ CMD_RIGCAT_TOGGLE_RTS_PTT,   SCRIPT_COMMAND, 0,  1, {0}, { p_bool },         0, 0, 0, process_rigcat_toggle_rts_ptt, 0, 0},
	{ CMD_RIGCAT_RESTORE,          SCRIPT_COMMAND, 0,  1, {0}, { p_bool },         0, 0, 0, process_rigcat_restore_on_close, 0, 0},
	{ CMD_RIGCAT_PTT_COMMAND,      SCRIPT_COMMAND, 0,  1, {0}, { p_bool },         0, 0, 0, process_rigcat_cat_command_ptt, 0, 0},
	{ CMD_RIGCAT_DTR_12V,          SCRIPT_COMMAND, 0,  1, {0}, { p_bool },         0, 0, 0, process_rigcat_dtr_12v, 0, 0},
	{ CMD_RIGCAT_VSP,              SCRIPT_COMMAND, 0,  1, {0}, { p_bool },         0, 0, 0, process_rigcat_vsp_enable, 0, 0},
	{ CMD_RIGCAT_INITIALIZE,       SCRIPT_COMMAND, 0,  1, {0}, { p_void },         0, 0, 0, process_rigcat_initialize, 0, 0},
	{ CMD_RIGCAT_END,              SCRIPT_COMMAND, 0,  0, {0}, { p_list_end },     0, 0, 0, 0, 0, 0},
	{ {0} }
};

static const SCRIPT_COMMANDS default_hamlib_command_table[] = {
	{ CMD_HAMLIB_STATE,            SCRIPT_COMMAND, 0,  1, {0}, { p_bool },         0, 0, 0, process_use_hamlib,              0, 0},
	{ CMD_HAMLIB_RIG,              SCRIPT_COMMAND, 0,  1, {0}, { p_dev_name },     0, 0, 0, process_hamlib_rig,              0, 0},
	{ CMD_HAMLIB_DEV_PATH,         SCRIPT_COMMAND, 0,  1, {0}, { p_dev_path },     0, 0, 0, process_hamlib_device_path,      0, 0},
	{ CMD_HAMLIB_RETRIES,          SCRIPT_COMMAND, 0,  1, {0}, { p_unsigned_int }, 0, 0, 0, process_hamlib_retries,          0, 0},
	{ CMD_HAMLIB_RETRY_INTERVAL,   SCRIPT_COMMAND, 0,  1, {0}, { p_unsigned_int }, 0, 0, 0, process_hamlib_retry_interval,   0, 0},
	{ CMD_HAMLIB_WRITE_DELAY,      SCRIPT_COMMAND, 0,  1, {0}, { p_unsigned_int }, 0, 0, 0, process_hamlib_write_delay,      0, 0},
	{ CMD_HAMLIB_POST_WRITE_DELAY, SCRIPT_COMMAND, 0,  1, {0}, { p_unsigned_int }, 0, 0, 0, process_hamlib_post_write_delay, 0, 0},
	{ CMD_HAMLIB_BAUD_RATE,        SCRIPT_COMMAND, 0,  1, {0}, { p_string },       0, 0, 0, process_hamlib_baud_rate,        0, 0},
	{ CMD_HAMLIB_STOP_BITS,        SCRIPT_COMMAND, 0,  1, {0}, { p_string },       0, 0, 0, process_hamlib_stop_bits,        0, 0},
	{ CMD_HAMLIB_SIDE_BAND,        SCRIPT_COMMAND, 0,  1, {0}, { p_string },       0, 0, 0, process_hamlib_sideband,         0, 0},
	{ CMD_HAMLIB_PTT_COMMAND,      SCRIPT_COMMAND, 0,  1, {0}, { p_bool   },       0, 0, 0, process_hamlib_ptt_hl_command,   0, 0},
	{ CMD_HAMLIB_DTR_12V,          SCRIPT_COMMAND, 0,  1, {0}, { p_bool   },       0, 0, 0, process_hamlib_dtr_12,           0, 0},
	{ CMD_HAMLIB_RTS_12V,          SCRIPT_COMMAND, 0,  1, {0}, { p_bool   },       0, 0, 0, process_hamlib_rts_12,           0, 0},
	{ CMD_HAMLIB_HRDWR_FLOW,       SCRIPT_COMMAND, 0,  1, {0}, { p_bool   },       0, 0, 0, process_hamlib_rts_cts_flow,     0, 0},
	{ CMD_HAMLIB_SFTWR_FLOW,       SCRIPT_COMMAND, 0,  1, {0}, { p_bool   },       0, 0, 0, process_hamlib_xon_xoff_flow,    0, 0},
	{ CMD_HAMLIB_ADV_CONFIG,       SCRIPT_COMMAND, 0,  1, {0}, { p_string },       0, 0, 0, process_hamlib_advanced_config,  0, 0},
	{ CMD_HAMLIB_INITIALIZE,       SCRIPT_COMMAND, 0,  1, {0}, { p_void   },       0, 0, 0, process_hamlib_initialize,       0, 0},
	{ CMD_HAMLIB_END,              SCRIPT_COMMAND, 0,  0, {0}, { p_list_end },     0, 0, 0, 0, 0, 0},
	{ {0} }
};

static const SCRIPT_COMMANDS default_rc_xmlrpc_command_table[] = {
	{ CMD_RC_XMLRPC_STATE,         SCRIPT_COMMAND, 0,  1, {0}, { p_bool  }, 0, 0, 0, process_use_xml_rpc,           0, 0},
	{ CMD_XMLRPC_BW_DELAY,         SCRIPT_COMMAND, 0,  1, {0}, { p_float }, 0, 0, 0, process_xml_rpc_mode_bw_delay, 0, 0},
	{ CMD_XMLRPC_INITIALIZE,       SCRIPT_COMMAND, 0,  1, {0}, { p_void  }, 0, 0, 0, process_xml_rpc_initialize,    0, 0},
	{ {0} }
};

static const SCRIPT_COMMANDS default_macro_command_table[] = {
	{ CMD_EXEC_MACRO_END, SCRIPT_COMMAND, 0,  0, {0}, { p_list_end }, 0, 0, 0, 0, 0, 0},
	{ {0} }
};

static const SCRIPT_COMMANDS default_top_level_command_table[] = {
	{ CMD_OPERATOR,         SCRIPT_COMMAND, 0,  1, {0}, { p_list   }, 0, 0, 0, 0, (struct script_cmds *) &default_operator_command_table,         sizeof(default_operator_command_table)/sizeof(SCRIPT_COMMANDS)},
	{ CMD_AUDIO_DEVICE,     SCRIPT_COMMAND, 0,  1, {0}, { p_list   }, 0, 0, 0, 0, (struct script_cmds *) &default_audio_device_command_table,     sizeof(default_audio_device_command_table)/sizeof(SCRIPT_COMMANDS)},
	{ CMD_AUDIO_SETTINGS,   SCRIPT_COMMAND, 0,  1, {0}, { p_list   }, 0, 0, 0, 0, (struct script_cmds *) &default_audio_settings_command_table,   sizeof(default_audio_settings_command_table)/sizeof(SCRIPT_COMMANDS)},
	{ CMD_AUDIO_RT_CHANNEL, SCRIPT_COMMAND, 0,  1, {0}, { p_list   }, 0, 0, 0, 0, (struct script_cmds *) &default_audio_rt_channel_command_table, sizeof(default_audio_rt_channel_command_table)/sizeof(SCRIPT_COMMANDS)},
	{ CMD_WAVE_SAMPLE_RATE, SCRIPT_COMMAND, 0,  1, {0}, { p_string }, 0, 0, 0, process_wave_file_sample_rate, 0, 0},
	{ CMD_HRDWR_PTT,        SCRIPT_COMMAND, 0,  1, {0}, { p_list   }, 0, 0, 0, 0, (struct script_cmds *) &default_rig_hrdwr_ptt_command_table,    sizeof(default_rig_hrdwr_ptt_command_table)/sizeof(SCRIPT_COMMANDS)},
	{ CMD_RIGCAT,           SCRIPT_COMMAND, 0,  1, {0}, { p_list   }, 0, 0, 0, 0, (struct script_cmds *) &default_rigcat_command_table,           sizeof(default_rigcat_command_table)/sizeof(SCRIPT_COMMANDS)},
	{ CMD_HAMLIB,           SCRIPT_COMMAND, 0,  1, {0}, { p_list   }, 0, 0, 0, 0, (struct script_cmds *) &default_hamlib_command_table,           sizeof(default_hamlib_command_table)/sizeof(SCRIPT_COMMANDS)},
	{ CMD_RC_XMLRPC,        SCRIPT_COMMAND, 0,  1, {0}, { p_list   }, 0, 0, 0, 0, (struct script_cmds *) &default_rc_xmlrpc_command_table,        sizeof(default_rc_xmlrpc_command_table)/sizeof(SCRIPT_COMMANDS)},
	{ CMD_EXEC_MACRO,       SCRIPT_COMMAND, 0,  1, {0}, { p_list   }, 0, 0, 0, 0, (struct script_cmds *) &default_macro_command_table,            sizeof(default_macro_command_table)/sizeof(SCRIPT_COMMANDS)},
	{ {0} }
};

/** **************************************************************
 * \brief Users Callsign
 * \param cmd Pointer to matching struct script_cmds script
 * command.
 * \return SCRIPT_CODES error see \ref script_codes
 * \par Script Command:<br>
 * <tt>\"CALLSIGN:\"</tt><br>
 * \par Script Parameters:<br>
 * <tt>String</tt>
 *****************************************************************/
SCRIPT_CODES ScriptParsing::sc_callsign_info(struct script_cmds *cmd)
{
	if(!cmd)
		return script_function_parameter_error;

	if(cmd->argc < 1 || !cmd->args[0])
		return script_invalid_parameter;

	this->callsign_info(cmd->args[0]);

	return script_no_errors;
}

/** **************************************************************
 * \brief Users Name
 * \param cmd Pointer to matching struct script_cmds script
 * command.
 \return SCRIPT_CODES error see \ref script_codes
 * \par Script Command:<br>
 * <tt>\"NAME:\"</tt><br>
 * \par Script Parameters:<br>
 * <tt>String</tt><br>
 *****************************************************************/
SCRIPT_CODES ScriptParsing::sc_name_info(struct script_cmds *cmd)
{
	if(!cmd)
		return script_function_parameter_error;

	if(cmd->argc < 1 || !cmd->args[0])
		return script_invalid_parameter;

	this->name_info(cmd->args[0]);

	return script_no_errors;
}

/** **************************************************************
 * \brief Users QTH
 * \param cmd Pointer to matching struct script_cmds script
 * command.
 \return SCRIPT_CODES error see \ref script_codes
 * \par Script Command:<br>
 * <tt>\"QTH:\"</tt><br>
 * \par Script Parameters:<br>
 * <tt>String</tt><br>
 *****************************************************************/
SCRIPT_CODES ScriptParsing::sc_qth_info(struct script_cmds *cmd)
{
	if(!cmd)
		return script_function_parameter_error;

	if(cmd->argc < 1 || !cmd->args[0])
		return script_invalid_parameter;

	this->qth_info(cmd->args[0]);

	return script_no_errors;
}

/** **************************************************************
 * \brief Users Locator
 * \param cmd Pointer to matching struct script_cmds script
 * command.
 \return SCRIPT_CODES error see \ref script_codes
 * \par Script Command:<br>
 * <tt>\"LOC:\"</tt><br>
 * \par Script Parameters:<br>
 * <tt>String</tt><br>
 *****************************************************************/
SCRIPT_CODES ScriptParsing::sc_locator_info(struct script_cmds *cmd)
{
	if(!cmd)
		return script_function_parameter_error;

	if(cmd->argc < 1 || !cmd->args[0])
		return script_invalid_parameter;

	this->locator_info(cmd->args[0]);

	return script_no_errors;
}

/** **************************************************************
 * \brief Antenna Description
 * \param cmd Pointer to matching struct script_cmds script
 * command.
 \return SCRIPT_CODES error see \ref script_codes
 * \par Script Command:<br>
 * <tt>\"ANT:\"</tt><br>
 * \par Script Parameters:<br>
 * <tt>String</tt><br>
 *****************************************************************/
SCRIPT_CODES ScriptParsing::sc_antenna_info(struct script_cmds *cmd)
{
	if(!cmd)
		return script_function_parameter_error;

	if(cmd->argc < 1 || !cmd->args[0])
		return script_invalid_parameter;

	this->antenna_info(cmd->args[0]);

	return script_no_errors;
}

/** **************************************************************
 * \brief Enable/Disable OSS Audio I/O
 * \param cmd Pointer to matching struct script_cmds script
 * command.
 \return SCRIPT_CODES error see \ref script_codes
 * \par Script Command:<br>
 * <tt>\"OSS:\"</tt><br>
 * \par Script Parameters:<br>
 * <tt>ENABLE, DISABLE</tt><br>
 *****************************************************************/
SCRIPT_CODES ScriptParsing::sc_use_oss_audio_device(struct script_cmds *cmd)
{
	if(!cmd)
		return script_function_parameter_error;

	if(cmd->argc < 1 || !cmd->args[0])
		return script_invalid_parameter;

	bool state = false;

	test_on_off_state(state, cmd->args[0], (char *)_("ENABLE"));
	this->use_oss_audio_device(state);

	return script_no_errors;
}

/** **************************************************************
 * \brief OSS device path
 * \param cmd Pointer to matching struct script_cmds script
 * command.
 \return SCRIPT_CODES error see \ref script_codes
 * \par Script Command:<br>
 * <tt>\"OSS PATH:\"</tt><br>
 * \par Script Parameters:<br>
 * <tt>Device Path</tt><br>
 *****************************************************************/
SCRIPT_CODES ScriptParsing::sc_oss_audio_device_path(struct script_cmds *cmd)
{
	if(!cmd)
		return script_function_parameter_error;

	if(cmd->argc < 1 || !cmd->args[0])
		return script_invalid_parameter;

	this->oss_audio_device_path(cmd->args[0]);

	return script_no_errors;
}

/** **************************************************************
 * \brief Enable/Disable Port Audio I/O
 * \param cmd Pointer to matching struct script_cmds script
 * command.
 \return SCRIPT_CODES error see \ref script_codes
 * \par Script Command:<br>
 * <tt>\"PA:\"</tt><br>
 * \par Script Parameters:<br>
 * <tt>ENABLE, DISABLE</tt><br>
 *****************************************************************/
SCRIPT_CODES ScriptParsing::sc_use_port_audio_device(struct script_cmds *cmd)
{
	if(!cmd)
		return script_function_parameter_error;

	if(cmd->argc < 1 || !cmd->args[0])
		return script_invalid_parameter;

	bool state = false;

	test_on_off_state(state, cmd->args[0], (char *)_("ENABLE"));
	this->use_port_audio_device(state);

	return script_no_errors;
}

/** **************************************************************
 * \brief Port Audio Capture Device Name
 * \param cmd Pointer to matching struct script_cmds script
 * command.
 \return SCRIPT_CODES error see \ref script_codes
 * \par Script Command:<br>
 * <tt>\"PA CAPTURE:\"</tt><br>
 * \par Script Parameters:<br>
 * <tt>Device Name</tt><br>
 *****************************************************************/
SCRIPT_CODES ScriptParsing::sc_capture_path(struct script_cmds *cmd)
{
	if(!cmd)
		return script_function_parameter_error;

	if(cmd->argc < 1 || !cmd->args[0])
		return script_invalid_parameter;

	this->capture_path(cmd->args[0]);

	return script_no_errors;
}

/** **************************************************************
 * \brief Port Audio Playback Device Name
 * \param cmd Pointer to matching struct script_cmds script
 * command.
 \return SCRIPT_CODES error see \ref script_codes
 * \par Script Command:<br>
 * <tt>\"PA PLAYBACK:\"</tt><br>
 * \par Script Parameters:<br>
 * <tt>Device Name</tt><br>
 *****************************************************************/
SCRIPT_CODES ScriptParsing::sc_playback_path(struct script_cmds *cmd)
{
	if(!cmd)
		return script_function_parameter_error;

	if(cmd->argc < 1 || !cmd->args[0])
		return script_invalid_parameter;

	this->playback_path(cmd->args[0]);

	return script_no_errors;
}

/** **************************************************************
 * \brief Enable/Disable Pulse Audio I/O
 * \param cmd Pointer to matching struct script_cmds script
 * command.
 \return SCRIPT_CODES error see \ref script_codes
 * \par Script Command:<br>
 * <tt>\"PUA:\"</tt><br>
 * \par Script Parameters:<br>
 * <tt>ENABLE, DISABLE</tt><br>
 *****************************************************************/
SCRIPT_CODES ScriptParsing::sc_use_pulse_audio_device(struct script_cmds *cmd)
{
	if(!cmd)
		return script_function_parameter_error;

	if(cmd->argc < 1 || !cmd->args[0])
		return script_invalid_parameter;

	bool state = false;

	test_on_off_state(state, cmd->args[0], (char *)_("ENABLE"));
	this->use_pulse_audio_device(state);

	return script_no_errors;
}

/** **************************************************************
 * \brief Pulse Audio device path
 * \param cmd Pointer to matching struct script_cmds script
 * command.
 \return SCRIPT_CODES error see \ref script_codes
 * \par Script Command:<br>
 * <tt>\"PUA PATH:\"</tt><br>
 * \par Script Parameters:<br>
 * <tt>Device Path</tt><br>
 *****************************************************************/
SCRIPT_CODES ScriptParsing::sc_pulse_audio_device_path(struct script_cmds *cmd)
{
	if(!cmd)
		return script_function_parameter_error;

	if(cmd->argc < 1 || !cmd->args[0])
		return script_invalid_parameter;

	this->pulse_audio_device_path(cmd->args[0]);

	return script_no_errors;
}

/** **************************************************************
 * \brief Audio Device Baud Rate - Capture
 * \param cmd Pointer to matching struct script_cmds script
 * command.
 \return SCRIPT_CODES error see \ref script_codes
 * \par Script Command:<br>
 * <tt>\"CSR:\"</tt><br>
 * \par Script Parameters:<br>
 * <tt>Auto, Native, 8000,...</tt><br>
 *****************************************************************/
SCRIPT_CODES ScriptParsing::sc_audio_device_sample_rate_capture(struct script_cmds *cmd)
{
	if(!cmd)
		return script_function_parameter_error;

	if(cmd->argc < 1 || !cmd->args[0])
		return script_invalid_parameter;

	this->audio_device_sample_rate_capture(cmd->args[0]);

	return script_no_errors;
}

/** **************************************************************
 * \brief Audio Device Baud Rate - Playback
 * \param cmd Pointer to matching struct script_cmds script
 * command.
 \return SCRIPT_CODES error see \ref script_codes
 * \par Script Command:<br>
 * <tt>\"PSR:\"</tt><br>
 * \par Script Parameters:<br>
 * <tt>Auto, Native, 8000,...</tt><br>
 *****************************************************************/
SCRIPT_CODES ScriptParsing::sc_audio_device_sample_rate_playback(struct script_cmds *cmd)
{
	if(!cmd)
		return script_function_parameter_error;

	if(cmd->argc < 1 || !cmd->args[0])
		return script_invalid_parameter;

	this->audio_device_sample_rate_playback(cmd->args[0]);

	return script_no_errors;
}

/** **************************************************************
 * \brief Sinc interpolator
 * \param cmd Pointer to matching struct script_cmds script
 * command.
 \return SCRIPT_CODES error see \ref script_codes
 * \par Script Command:<br>
 * <tt>\"CONVERTER:\"</tt><br>
 * \par Script Parameters:<br>
 * <tt>Best, Medium, Fast, Linear</tt><br>
 *****************************************************************/
SCRIPT_CODES ScriptParsing::sc_audio_device_converter(struct script_cmds *cmd)
{
	if(!cmd)
		return script_function_parameter_error;

	if(cmd->argc < 1 || !cmd->args[0])
		return script_invalid_parameter;

	this->audio_device_converter(cmd->args[0]);

	return script_no_errors;
}

/** **************************************************************
 * \brief Receive sample rate error correction
 * \param cmd Pointer to matching struct script_cmds script
 * command.
 \return SCRIPT_CODES error see \ref script_codes
 * \par Script Command:<br>
 * <tt>\"RX PPM:\"</tt><br>
 * \par Script Parameters:<br>
 * <tt>Integer</tt><br>
 *****************************************************************/
SCRIPT_CODES ScriptParsing::sc_rx_ppm(struct script_cmds *cmd)
{
	if(!cmd)
		return script_function_parameter_error;

	if(cmd->argc < 1 || !cmd->args[0])
		return script_invalid_parameter;

	int value = 0;
	int test = sscanf(cmd->args[0], "%d", &value);

	if(!test)
		return script_invalid_parameter;

	this->rx_ppm(value);

	return script_no_errors;
}

/** **************************************************************
 * \brief Transmit sample rate error correction
 * \param cmd Pointer to matching struct script_cmds script
 * command.
 \return SCRIPT_CODES error see \ref script_codes
 * \par Script Command:<br>
 * <tt>\"TX PPM:\"</tt><br>
 * \par Script Parameters:<br>
 * <tt>Integer</tt><br>
 *****************************************************************/
SCRIPT_CODES ScriptParsing::sc_tx_ppm(struct script_cmds *cmd)
{
	if(!cmd)
		return script_function_parameter_error;

	if(cmd->argc < 1 || !cmd->args[0])
		return script_invalid_parameter;

	int value = 0;
	int test = sscanf(cmd->args[0], "%d", &value);

	if(!test)
		return script_invalid_parameter;

	this->tx_ppm(value);

	return script_no_errors;
}

/** **************************************************************
 * \brief Transmit frequency offset in Hz
 * \param cmd Pointer to matching struct script_cmds script
 * command.
 \return SCRIPT_CODES error see \ref script_codes
 * \par Script Command:<br>
 * <tt>\"TX OFFSET:\"</tt><br>
 * \par Script Parameters:<br>
 * <tt>Integer</tt><br>
 *****************************************************************/
SCRIPT_CODES ScriptParsing::sc_tx_offset(struct script_cmds *cmd)
{
	if(!cmd)
		return script_function_parameter_error;

	if(cmd->argc < 1 || !cmd->args[0])
		return script_invalid_parameter;

	int value = 0;
	int test = sscanf(cmd->args[0], "%d", &value);

	if(!test)
		return script_invalid_parameter;

	this->tx_offset(value);

	return script_no_errors;
}

/** **************************************************************
 * \brief Enable Mono Audio output
 * \param cmd Pointer to matching struct script_cmds script
 * command.
 \return SCRIPT_CODES error see \ref script_codes
 * \par Script Command:<br>
 * <tt>\"MONO AUDIO:\"</tt><br>
 * \par Script Parameters:<br>
 * <tt>ENABLE, DISABLE</tt><br>
 *****************************************************************/
SCRIPT_CODES ScriptParsing::sc_mono_audio_output(struct script_cmds *cmd)
{
	if(!cmd)
		return script_function_parameter_error;

	if(cmd->argc < 1 || !cmd->args[0])
		return script_invalid_parameter;

	bool state = false;

	test_on_off_state(state, cmd->args[0], (char *)_("ENABLE"));
	this->mono_audio_output(state);

	return script_no_errors;
}

/** **************************************************************
 * \brief Enable Left/Right Audio output
 * \param cmd Pointer to matching struct script_cmds script
 * command.
 \return SCRIPT_CODES error see \ref script_codes
 * \par Script Command:<br>
 * <tt>\"AUDIO LR:\"</tt><br>
 * \par Script Parameters:<br>
 * <tt>ENABLE, DISABLE</tt><br>
 *****************************************************************/
SCRIPT_CODES ScriptParsing::sc_modem_signal_left_right(struct script_cmds *cmd)
{
	if(!cmd)
		return script_function_parameter_error;

	if(cmd->argc < 1 || !cmd->args[0])
		return script_invalid_parameter;

	bool state = false;

	test_on_off_state(state, cmd->args[0], (char *)_("ENABLE"));
	this->modem_signal_left_right(state);

	return script_no_errors;
}

/** **************************************************************
 * \brief Enable Left/Right Audio output reversed.
 * \param cmd Pointer to matching struct script_cmds script
 * command.
 \return SCRIPT_CODES error see \ref script_codes
 * \par Script Command:<br>
 * <tt>\"AUDIO REV LR:\"</tt><br>
 * \par Script Parameters:<br>
 * <tt>ENABLE, DISABLE</tt><br>
 *****************************************************************/
SCRIPT_CODES ScriptParsing::sc_reverse_left_right(struct script_cmds *cmd)
{
	if(!cmd)
		return script_function_parameter_error;

	if(cmd->argc < 1 || !cmd->args[0])
		return script_invalid_parameter;

	bool state = false;

	test_on_off_state(state, cmd->args[0], (char *)_("ENABLE"));
	this->modem_signal_left_right(state);

	return script_no_errors;
}

/** **************************************************************
 * \brief Enable/Disable PTT option on right audio channel
 * \param cmd Pointer to matching struct script_cmds script
 * command.
 \return SCRIPT_CODES error see \ref script_codes
 * \par Script Command:<br>
 * <tt>\"PTT RT CHAN:\"</tt><br>
 * \par Script Parameters:<br>
 * <tt>ENABLE, DISABLE</tt><br>
 *****************************************************************/
SCRIPT_CODES ScriptParsing::sc_ptt_tone_right_channel(struct script_cmds *cmd)
{
	if(!cmd)
		return script_function_parameter_error;

	if(cmd->argc < 1 || !cmd->args[0])
		return script_invalid_parameter;

	bool state = false;

	test_on_off_state(state, cmd->args[0], (char *)_("ENABLE"));
	this->ptt_tone_right_channel(state);

	return script_no_errors;
}

/** **************************************************************
 * \brief Remove all files in the transmit queue.
 * \param cmd Pointer to matching struct script_cmds script
 * command.
 \return SCRIPT_CODES error see \ref script_codes
 * \par Script Command:<br>
 * <tt>\"CLEAR TXQ:\"</tt><br>
 * \par Script Parameters:<br>
 * <tt>None</tt><br>
 *****************************************************************/
SCRIPT_CODES ScriptParsing::sc_cw_qsk_right_channel(struct script_cmds *cmd)
{
	return script_no_errors;
}

/** **************************************************************
 * \brief Remove all files in the transmit queue.
 * \param cmd Pointer to matching struct script_cmds script
 * command.
 \return SCRIPT_CODES error see \ref script_codes
 * \par Script Command:<br>
 * <tt>\"CLEAR TXQ:\"</tt><br>
 * \par Script Parameters:<br>
 * <tt>None</tt><br>
 *****************************************************************/
SCRIPT_CODES ScriptParsing::sc_pseudo_fsk_right_channel(struct script_cmds *cmd)
{
	return script_no_errors;
}

/** **************************************************************
 * \brief Remove all files in the transmit queue.
 * \param cmd Pointer to matching struct script_cmds script
 * command.
 \return SCRIPT_CODES error see \ref script_codes
 * \par Script Command:<br>
 * <tt>\"CLEAR TXQ:\"</tt><br>
 * \par Script Parameters:<br>
 * <tt>None</tt><br>
 *****************************************************************/
SCRIPT_CODES ScriptParsing::sc_wave_file_sample_rate(struct script_cmds *cmd)
{
	return script_no_errors;
}

/** **************************************************************
 * \brief Remove all files in the transmit queue.
 * \param cmd Pointer to matching struct script_cmds script
 * command.
 \return SCRIPT_CODES error see \ref script_codes
 * \par Script Command:<br>
 * <tt>\"CLEAR TXQ:\"</tt><br>
 * \par Script Parameters:<br>
 * <tt>None</tt><br>
 *****************************************************************/
SCRIPT_CODES ScriptParsing::sc_hrdw_ptt_right_audio_channel(struct script_cmds *cmd)
{
	return script_no_errors;
}

/** **************************************************************
 * \brief Remove all files in the transmit queue.
 * \param cmd Pointer to matching struct script_cmds script
 * command.
 \return SCRIPT_CODES error see \ref script_codes
 * \par Script Command:<br>
 * <tt>\"CLEAR TXQ:\"</tt><br>
 * \par Script Parameters:<br>
 * <tt>None</tt><br>
 *****************************************************************/
SCRIPT_CODES ScriptParsing::sc_hrdw_ptt_sep_serial_port(struct script_cmds *cmd)
{
	return script_no_errors;
}

/** **************************************************************
 * \brief Remove all files in the transmit queue.
 * \param cmd Pointer to matching struct script_cmds script
 * command.
 \return SCRIPT_CODES error see \ref script_codes
 * \par Script Command:<br>
 * <tt>\"CLEAR TXQ:\"</tt><br>
 * \par Script Parameters:<br>
 * <tt>None</tt><br>
 *****************************************************************/
SCRIPT_CODES ScriptParsing::sc_hrdw_ptt_sep_serial_port_path(struct script_cmds *cmd)
{
	return script_no_errors;
}

/** **************************************************************
 * \brief Remove all files in the transmit queue.
 * \param cmd Pointer to matching struct script_cmds script
 * command.
 \return SCRIPT_CODES error see \ref script_codes
 * \par Script Command:<br>
 * <tt>\"CLEAR TXQ:\"</tt><br>
 * \par Script Parameters:<br>
 * <tt>None</tt><br>
 *****************************************************************/
SCRIPT_CODES ScriptParsing::sc_hrdw_ptt_sep_serial_port_rts(struct script_cmds *cmd)
{
	return script_no_errors;
}

/** **************************************************************
 * \brief Remove all files in the transmit queue.
 * \param cmd Pointer to matching struct script_cmds script
 * command.
 \return SCRIPT_CODES error see \ref script_codes
 * \par Script Command:<br>
 * <tt>\"CLEAR TXQ:\"</tt><br>
 * \par Script Parameters:<br>
 * <tt>None</tt><br>
 *****************************************************************/
SCRIPT_CODES ScriptParsing::sc_hrdw_ptt_sep_serial_port_dtr(struct script_cmds *cmd)
{
	return script_no_errors;
}

/** **************************************************************
 * \brief Remove all files in the transmit queue.
 * \param cmd Pointer to matching struct script_cmds script
 * command.
 \return SCRIPT_CODES error see \ref script_codes
 * \par Script Command:<br>
 * <tt>\"CLEAR TXQ:\"</tt><br>
 * \par Script Parameters:<br>
 * <tt>None</tt><br>
 *****************************************************************/
SCRIPT_CODES ScriptParsing::sc_hrdw_ptt_sep_serial_port_rts_v(struct script_cmds *cmd)
{
	return script_no_errors;
}

/** **************************************************************
 * \brief Remove all files in the transmit queue.
 * \param cmd Pointer to matching struct script_cmds script
 * command.
 \return SCRIPT_CODES error see \ref script_codes
 * \par Script Command:<br>
 * <tt>\"CLEAR TXQ:\"</tt><br>
 * \par Script Parameters:<br>
 * <tt>None</tt><br>
 *****************************************************************/
SCRIPT_CODES ScriptParsing::sc_hrdw_ptt_sep_serial_port_dtr_v(struct script_cmds *cmd)
{
	return script_no_errors;
}

/** **************************************************************
 * \brief Remove all files in the transmit queue.
 * \param cmd Pointer to matching struct script_cmds script
 * command.
 \return SCRIPT_CODES error see \ref script_codes
 * \par Script Command:<br>
 * <tt>\"CLEAR TXQ:\"</tt><br>
 * \par Script Parameters:<br>
 * <tt>None</tt><br>
 *****************************************************************/
SCRIPT_CODES ScriptParsing::sc_hrdw_ptt_start_delay(struct script_cmds *cmd)
{
	return script_no_errors;
}

/** **************************************************************
 * \brief Remove all files in the transmit queue.
 * \param cmd Pointer to matching struct script_cmds script
 * command.
 \return SCRIPT_CODES error see \ref script_codes
 * \par Script Command:<br>
 * <tt>\"CLEAR TXQ:\"</tt><br>
 * \par Script Parameters:<br>
 * <tt>None</tt><br>
 *****************************************************************/
SCRIPT_CODES ScriptParsing::sc_hrdw_ptt_end_delay(struct script_cmds *cmd)
{
	return script_no_errors;
}

/** **************************************************************
 * \brief Remove all files in the transmit queue.
 * \param cmd Pointer to matching struct script_cmds script
 * command.
 \return SCRIPT_CODES error see \ref script_codes
 * \par Script Command:<br>
 * <tt>\"CLEAR TXQ:\"</tt><br>
 * \par Script Parameters:<br>
 * <tt>None</tt><br>
 *****************************************************************/
SCRIPT_CODES ScriptParsing::sc_hrdw_ptt_initialize(struct script_cmds *cmd)
{
	return script_no_errors;
}

/** **************************************************************
 * \brief Remove all files in the transmit queue.
 * \param cmd Pointer to matching struct script_cmds script
 * command.
 \return SCRIPT_CODES error see \ref script_codes
 * \par Script Command:<br>
 * <tt>\"CLEAR TXQ:\"</tt><br>
 * \par Script Parameters:<br>
 * <tt>None</tt><br>
 *****************************************************************/
SCRIPT_CODES ScriptParsing::sc_use_rigcat(struct script_cmds *cmd)
{
	return script_no_errors;
}

/** **************************************************************
 * \brief Remove all files in the transmit queue.
 * \param cmd Pointer to matching struct script_cmds script
 * command.
 \return SCRIPT_CODES error see \ref script_codes
 * \par Script Command:<br>
 * <tt>\"CLEAR TXQ:\"</tt><br>
 * \par Script Parameters:<br>
 * <tt>None</tt><br>
 *****************************************************************/
SCRIPT_CODES ScriptParsing::sc_rigcat_desc_file(struct script_cmds *cmd)
{
	return script_no_errors;
}

/** **************************************************************
 * \brief Remove all files in the transmit queue.
 * \param cmd Pointer to matching struct script_cmds script
 * command.
 \return SCRIPT_CODES error see \ref script_codes
 * \par Script Command:<br>
 * <tt>\"CLEAR TXQ:\"</tt><br>
 * \par Script Parameters:<br>
 * <tt>None</tt><br>
 *****************************************************************/
SCRIPT_CODES ScriptParsing::sc_rigcat_device_path(struct script_cmds *cmd)
{
	return script_no_errors;
}

/** **************************************************************
 * \brief Remove all files in the transmit queue.
 * \param cmd Pointer to matching struct script_cmds script
 * command.
 \return SCRIPT_CODES error see \ref script_codes
 * \par Script Command:<br>
 * <tt>\"CLEAR TXQ:\"</tt><br>
 * \par Script Parameters:<br>
 * <tt>None</tt><br>
 *****************************************************************/
SCRIPT_CODES ScriptParsing::sc_rigcat_retries(struct script_cmds *cmd)
{
	return script_no_errors;
}

/** **************************************************************
 * \brief
 * \param cmd Pointer to matching struct script_cmds script
 * command.
 \return SCRIPT_CODES error see \ref script_codes
 * \par Script Command:<br>
 * <tt>\"CLEAR TXQ:\"</tt><br>
 * \par Script Parameters:<br>
 * <tt>None</tt><br>
 *****************************************************************/
SCRIPT_CODES ScriptParsing::sc_rigcat_retry_interval(struct script_cmds *cmd)
{
	return script_no_errors;
}

/** **************************************************************
 * \brief Remove all files in the transmit queue.
 * \param cmd Pointer to matching struct script_cmds script
 * command.
 \return SCRIPT_CODES error see \ref script_codes
 * \par Script Command:<br>
 * <tt>\"CLEAR TXQ:\"</tt><br>
 * \par Script Parameters:<br>
 * <tt>None</tt><br>
 *****************************************************************/
SCRIPT_CODES ScriptParsing::sc_rigcat_write_delay(struct script_cmds *cmd)
{
	return script_no_errors;
}

/** **************************************************************
 * \brief Remove all files in the transmit queue.
 * \param cmd Pointer to matching struct script_cmds script
 * command.
 \return SCRIPT_CODES error see \ref script_codes
 * \par Script Command:<br>
 * <tt>\"CLEAR TXQ:\"</tt><br>
 * \par Script Parameters:<br>
 * <tt>None</tt><br>
 *****************************************************************/
SCRIPT_CODES ScriptParsing::sc_rigcat_init_delay(struct script_cmds *cmd)
{
	return script_no_errors;
}

/** **************************************************************
 * \brief Remove all files in the transmit queue.
 * \param cmd Pointer to matching struct script_cmds script
 * command.
 \return SCRIPT_CODES error see \ref script_codes
 * \par Script Command:<br>
 * <tt>\"CLEAR TXQ:\"</tt><br>
 * \par Script Parameters:<br>
 * <tt>None</tt><br>
 *****************************************************************/
SCRIPT_CODES ScriptParsing::sc_rigcat_baud_rate(struct script_cmds *cmd)
{
	return script_no_errors;
}

/** **************************************************************
 * \brief Remove all files in the transmit queue.
 * \param cmd Pointer to matching struct script_cmds script
 * command.
 \return SCRIPT_CODES error see \ref script_codes
 * \par Script Command:<br>
 * <tt>\"CLEAR TXQ:\"</tt><br>
 * \par Script Parameters:<br>
 * <tt>None</tt><br>
 *****************************************************************/
SCRIPT_CODES ScriptParsing::sc_rigcat_stop_bits(struct script_cmds *cmd)
{
	return script_no_errors;
}

/** **************************************************************
 * \brief Remove all files in the transmit queue.
 * \param cmd Pointer to matching struct script_cmds script
 * command.
 \return SCRIPT_CODES error see \ref script_codes
 * \par Script Command:<br>
 * <tt>\"CLEAR TXQ:\"</tt><br>
 * \par Script Parameters:<br>
 * <tt>None</tt><br>
 *****************************************************************/
SCRIPT_CODES ScriptParsing::sc_rigcat_commands_echoed(struct script_cmds *cmd)
{
	return script_no_errors;
}

/** **************************************************************
 * \brief Remove all files in the transmit queue.
 * \param cmd Pointer to matching struct script_cmds script
 * command.
 \return SCRIPT_CODES error see \ref script_codes
 * \par Script Command:<br>
 * <tt>\"CLEAR TXQ:\"</tt><br>
 * \par Script Parameters:<br>
 * <tt>None</tt><br>
 *****************************************************************/
SCRIPT_CODES ScriptParsing::sc_rigcat_toggle_rts_ptt(struct script_cmds *cmd)
{
	return script_no_errors;
}

/** **************************************************************
 * \brief Remove all files in the transmit queue.
 * \param cmd Pointer to matching struct script_cmds script
 * command.
 \return SCRIPT_CODES error see \ref script_codes
 * \par Script Command:<br>
 * <tt>\"CLEAR TXQ:\"</tt><br>
 * \par Script Parameters:<br>
 * <tt>None</tt><br>
 *****************************************************************/
SCRIPT_CODES ScriptParsing::sc_rigcat_restore_on_close(struct script_cmds *cmd)
{
	return script_no_errors;
}

/** **************************************************************
 * \brief Remove all files in the transmit queue.
 * \param cmd Pointer to matching struct script_cmds script
 * command.
 \return SCRIPT_CODES error see \ref script_codes
 * \par Script Command:<br>
 * <tt>\"CLEAR TXQ:\"</tt><br>
 * \par Script Parameters:<br>
 * <tt>None</tt><br>
 *****************************************************************/
SCRIPT_CODES ScriptParsing::sc_rigcat_cat_command_ptt(struct script_cmds *cmd)
{
	return script_no_errors;
}

/** **************************************************************
 * \brief Remove all files in the transmit queue.
 * \param cmd Pointer to matching struct script_cmds script
 * command.
 \return SCRIPT_CODES error see \ref script_codes
 * \par Script Command:<br>
 * <tt>\"CLEAR TXQ:\"</tt><br>
 * \par Script Parameters:<br>
 * <tt>None</tt><br>
 *****************************************************************/
SCRIPT_CODES ScriptParsing::sc_rigcat_dtr_12v(struct script_cmds *cmd)
{
	return script_no_errors;
}

/** **************************************************************
 * \brief Remove all files in the transmit queue.
 * \param cmd Pointer to matching struct script_cmds script
 * command.
 \return SCRIPT_CODES error see \ref script_codes
 * \par Script Command:<br>
 * <tt>\"CLEAR TXQ:\"</tt><br>
 * \par Script Parameters:<br>
 * <tt>None</tt><br>
 *****************************************************************/
SCRIPT_CODES ScriptParsing::sc_rigcat_vsp_enable(struct script_cmds *cmd)
{
	return script_no_errors;
}

/** **************************************************************
 * \brief Remove all files in the transmit queue.
 * \param cmd Pointer to matching struct script_cmds script
 * command.
 \return SCRIPT_CODES error see \ref script_codes
 * \par Script Command:<br>
 * <tt>\"CLEAR TXQ:\"</tt><br>
 * \par Script Parameters:<br>
 * <tt>None</tt><br>
 *****************************************************************/
SCRIPT_CODES ScriptParsing::sc_rigcat_initialize(struct script_cmds *cmd)
{
	return script_no_errors;
}

/** **************************************************************
 * \brief Remove all files in the transmit queue.
 * \param cmd Pointer to matching struct script_cmds script
 * command.
 \return SCRIPT_CODES error see \ref script_codes
 * \par Script Command:<br>
 * <tt>\"CLEAR TXQ:\"</tt><br>
 * \par Script Parameters:<br>
 * <tt>None</tt><br>
 *****************************************************************/
SCRIPT_CODES ScriptParsing::sc_use_hamlib(struct script_cmds *cmd)
{
	return script_no_errors;
}

/** **************************************************************
 * \brief Remove all files in the transmit queue.
 * \param cmd Pointer to matching struct script_cmds script
 * command.
 \return SCRIPT_CODES error see \ref script_codes
 * \par Script Command:<br>
 * <tt>\"CLEAR TXQ:\"</tt><br>
 * \par Script Parameters:<br>
 * <tt>None</tt><br>
 *****************************************************************/
SCRIPT_CODES ScriptParsing::sc_hamlib_rig(struct script_cmds *cmd)
{
	return script_no_errors;
}

/** **************************************************************
 * \brief Remove all files in the transmit queue.
 * \param cmd Pointer to matching struct script_cmds script
 * command.
 \return SCRIPT_CODES error see \ref script_codes
 * \par Script Command:<br>
 * <tt>\"CLEAR TXQ:\"</tt><br>
 * \par Script Parameters:<br>
 * <tt>None</tt><br>
 *****************************************************************/
SCRIPT_CODES ScriptParsing::sc_hamlib_retries(struct script_cmds *cmd)
{
	return script_no_errors;
}

/** **************************************************************
 * \brief Remove all files in the transmit queue.
 * \param cmd Pointer to matching struct script_cmds script
 * command.
 \return SCRIPT_CODES error see \ref script_codes
 * \par Script Command:<br>
 * <tt>\"CLEAR TXQ:\"</tt><br>
 * \par Script Parameters:<br>
 * <tt>None</tt><br>
 *****************************************************************/
SCRIPT_CODES ScriptParsing::sc_hamlib_retry_interval(struct script_cmds *cmd)
{
	return script_no_errors;
}

/** **************************************************************
 * \brief Remove all files in the transmit queue.
 * \param cmd Pointer to matching struct script_cmds script
 * command.
 \return SCRIPT_CODES error see \ref script_codes
 * \par Script Command:<br>
 * <tt>\"CLEAR TXQ:\"</tt><br>
 * \par Script Parameters:<br>
 * <tt>None</tt><br>
 *****************************************************************/
SCRIPT_CODES ScriptParsing::sc_hamlib_write_delay(struct script_cmds *cmd)
{
	return script_no_errors;
}

/** **************************************************************
 * \brief Remove all files in the transmit queue.
 * \param cmd Pointer to matching struct script_cmds script
 * command.
 \return SCRIPT_CODES error see \ref script_codes
 * \par Script Command:<br>
 * <tt>\"CLEAR TXQ:\"</tt><br>
 * \par Script Parameters:<br>
 * <tt>None</tt><br>
 *****************************************************************/
SCRIPT_CODES ScriptParsing::sc_hamlib_post_write_delay(struct script_cmds *cmd)
{
	return script_no_errors;
}

/** **************************************************************
 * \brief Remove all files in the transmit queue.
 * \param cmd Pointer to matching struct script_cmds script
 * command.
 \return SCRIPT_CODES error see \ref script_codes
 * \par Script Command:<br>
 * <tt>\"CLEAR TXQ:\"</tt><br>
 * \par Script Parameters:<br>
 * <tt>None</tt><br>
 *****************************************************************/
SCRIPT_CODES ScriptParsing::sc_hamlib_device_path(struct script_cmds *cmd)
{
	return script_no_errors;
}

/** **************************************************************
 * \brief Remove all files in the transmit queue.
 * \param cmd Pointer to matching struct script_cmds script
 * command.
 \return SCRIPT_CODES error see \ref script_codes
 * \par Script Command:<br>
 * <tt>\"CLEAR TXQ:\"</tt><br>
 * \par Script Parameters:<br>
 * <tt>None</tt><br>
 *****************************************************************/
SCRIPT_CODES ScriptParsing::sc_hamlib_baud_rate(struct script_cmds *cmd)
{
	return script_no_errors;
}

/** **************************************************************
 * \brief Remove all files in the transmit queue.
 * \param cmd Pointer to matching struct script_cmds script
 * command.
 \return SCRIPT_CODES error see \ref script_codes
 * \par Script Command:<br>
 * <tt>\"CLEAR TXQ:\"</tt><br>
 * \par Script Parameters:<br>
 * <tt>None</tt><br>
 *****************************************************************/
SCRIPT_CODES ScriptParsing::sc_hamlib_stop_bits(struct script_cmds *cmd)
{
	return script_no_errors;
}

/** **************************************************************
 * \brief Remove all files in the transmit queue.
 * \param cmd Pointer to matching struct script_cmds script
 * command.
 \return SCRIPT_CODES error see \ref script_codes
 * \par Script Command:<br>
 * <tt>\"CLEAR TXQ:\"</tt><br>
 * \par Script Parameters:<br>
 * <tt>None</tt><br>
 *****************************************************************/
SCRIPT_CODES ScriptParsing::sc_hamlib_sideband(struct script_cmds *cmd)
{
	return script_no_errors;
}

/** **************************************************************
 * \brief Remove all files in the transmit queue.
 * \param cmd Pointer to matching struct script_cmds script
 * command.
 \return SCRIPT_CODES error see \ref script_codes
 * \par Script Command:<br>
 * <tt>\"CLEAR TXQ:\"</tt><br>
 * \par Script Parameters:<br>
 * <tt>None</tt><br>
 *****************************************************************/
SCRIPT_CODES ScriptParsing::sc_hamlib_ptt_hl_command(struct script_cmds *cmd)
{
	return script_no_errors;
}

/** **************************************************************
 * \brief Remove all files in the transmit queue.
 * \param cmd Pointer to matching struct script_cmds script
 * command.
 \return SCRIPT_CODES error see \ref script_codes
 * \par Script Command:<br>
 * <tt>\"CLEAR TXQ:\"</tt><br>
 * \par Script Parameters:<br>
 * <tt>None</tt><br>
 *****************************************************************/
SCRIPT_CODES ScriptParsing::sc_hamlib_dtr_12(struct script_cmds *cmd)
{
	return script_no_errors;
}

/** **************************************************************
 * \brief Remove all files in the transmit queue.
 * \param cmd Pointer to matching struct script_cmds script
 * command.
 \return SCRIPT_CODES error see \ref script_codes
 * \par Script Command:<br>
 * <tt>\"CLEAR TXQ:\"</tt><br>
 * \par Script Parameters:<br>
 * <tt>None</tt><br>
 *****************************************************************/
SCRIPT_CODES ScriptParsing::sc_hamlib_rts_12(struct script_cmds *cmd)
{
	return script_no_errors;
}

/** **************************************************************
 * \brief Remove all files in the transmit queue.
 * \param cmd Pointer to matching struct script_cmds script
 * command.
 \return SCRIPT_CODES error see \ref script_codes
 * \par Script Command:<br>
 * <tt>\"CLEAR TXQ:\"</tt><br>
 * \par Script Parameters:<br>
 * <tt>None</tt><br>
 *****************************************************************/
SCRIPT_CODES ScriptParsing::sc_hamlib_rts_cts_flow(struct script_cmds *cmd)
{
	return script_no_errors;
}

/** **************************************************************
 * \brief Remove all files in the transmit queue.
 * \param cmd Pointer to matching struct script_cmds script
 * command.
 \return SCRIPT_CODES error see \ref script_codes
 * \par Script Command:<br>
 * <tt>\"CLEAR TXQ:\"</tt><br>
 * \par Script Parameters:<br>
 * <tt>None</tt><br>
 *****************************************************************/
SCRIPT_CODES ScriptParsing::sc_hamlib_xon_xoff_flow(struct script_cmds *cmd)
{
	return script_no_errors;
}

/** **************************************************************
 * \brief Remove all files in the transmit queue.
 * \param cmd Pointer to matching struct script_cmds script
 * command.
 \return SCRIPT_CODES error see \ref script_codes
 * \par Script Command:<br>
 * <tt>\"CLEAR TXQ:\"</tt><br>
 * \par Script Parameters:<br>
 * <tt>None</tt><br>
 *****************************************************************/
SCRIPT_CODES ScriptParsing::sc_hamlib_advanced_config(struct script_cmds *cmd)
{
	return script_no_errors;
}

/** **************************************************************
 * \brief Remove all files in the transmit queue.
 * \param cmd Pointer to matching struct script_cmds script
 * command.
 \return SCRIPT_CODES error see \ref script_codes
 * \par Script Command:<br>
 * <tt>\"CLEAR TXQ:\"</tt><br>
 * \par Script Parameters:<br>
 * <tt>None</tt><br>
 *****************************************************************/
SCRIPT_CODES ScriptParsing::sc_hamlib_initialize(struct script_cmds *cmd)
{
	return script_no_errors;
}

/** **************************************************************
 * \brief Remove all files in the transmit queue.
 * \param cmd Pointer to matching struct script_cmds script
 * command.
 \return SCRIPT_CODES error see \ref script_codes
 * \par Script Command:<br>
 * <tt>\"CLEAR TXQ:\"</tt><br>
 * \par Script Parameters:<br>
 * <tt>None</tt><br>
 *****************************************************************/
SCRIPT_CODES ScriptParsing::sc_use_xml_rpc(struct script_cmds *cmd)
{
	return script_no_errors;
}

/** **************************************************************
 * \brief Remove all files in the transmit queue.
 * \param cmd Pointer to matching struct script_cmds script
 * command.
 \return SCRIPT_CODES error see \ref script_codes
 * \par Script Command:<br>
 * <tt>\"CLEAR TXQ:\"</tt><br>
 * \par Script Parameters:<br>
 * <tt>None</tt><br>
 *****************************************************************/
SCRIPT_CODES ScriptParsing::sc_xml_rpc_mode_bw_delay(struct script_cmds *cmd)
{
	return script_no_errors;
}

/** **************************************************************
 * \brief Remove all files in the transmit queue.
 * \param cmd Pointer to matching struct script_cmds script
 * command.
 \return SCRIPT_CODES error see \ref script_codes
 * \par Script Command:<br>
 * <tt>\"CLEAR TXQ:\"</tt><br>
 * \par Script Parameters:<br>
 * <tt>None</tt><br>
 *****************************************************************/
SCRIPT_CODES ScriptParsing::sc_xml_rpc_initialize(struct script_cmds *cmd)
{
	return script_no_errors;
}

/*****************************************************************
 *****************************************************************
 Upper Level calling members
 *****************************************************************
 *****************************************************************/

/** **************************************************************
 * \brief Remove all files in the transmit queue.
 * \param cmd Pointer to matching struct script_cmds script
 * command.
 \return SCRIPT_CODES error see \ref script_codes
 * \par Script Command:<br>
 * <tt>\"CLEAR TXQ:\"</tt><br>
 * \par Script Parameters:<br>
 * <tt>None</tt><br>
 *****************************************************************/
SCRIPT_CODES ScriptParsing::sc_operator(struct script_cmds *)
{
	return script_no_errors;
}

/** **************************************************************
 * \brief Remove all files in the transmit queue.
 * \param cmd Pointer to matching struct script_cmds script
 * command.
 \return SCRIPT_CODES error see \ref script_codes
 * \par Script Command:<br>
 * <tt>\"CLEAR TXQ:\"</tt><br>
 * \par Script Parameters:<br>
 * <tt>None</tt><br>
 *****************************************************************/
SCRIPT_CODES ScriptParsing::sc_audio_device(struct script_cmds *cmd)
{
	return script_no_errors;
}


/** **************************************************************
 * \brief Remove all files in the transmit queue.
 * \param cmd Pointer to matching struct script_cmds script
 * command.
 \return SCRIPT_CODES error see \ref script_codes
 * \par Script Command:<br>
 * <tt>\"CLEAR TXQ:\"</tt><br>
 * \par Script Parameters:<br>
 * <tt>None</tt><br>
 *****************************************************************/
SCRIPT_CODES ScriptParsing::sc_audio_settings(struct script_cmds *cmd)
{
	return script_no_errors;
}


/** **************************************************************
 * \brief Audio Right Channel Top level processing
 * \param cmd Pointer to matching struct script_cmds script
 * command.
 \return SCRIPT_CODES error see \ref script_codes
 * \par Script Command:<br>
 * <tt>\"CLEAR TXQ:\"</tt><br>
 * \par Script Parameters:<br>
 * <tt>None</tt><br>
 *****************************************************************/
SCRIPT_CODES ScriptParsing::sc_audio_rt_channel(struct script_cmds *cmd)
{
	return script_no_errors;
}


/** **************************************************************
 * \brief Remove all files in the transmit queue.
 * \param cmd Pointer to matching struct script_cmds script
 * command.
 \return SCRIPT_CODES error see \ref script_codes
 * \par Script Command:<br>
 * <tt>\"CLEAR TXQ:\"</tt><br>
 * \par Script Parameters:<br>
 * <tt>None</tt><br>
 *****************************************************************/
SCRIPT_CODES ScriptParsing::sc_rigcat(struct script_cmds *cmd)
{
	return script_no_errors;
}

/** **************************************************************
 * \brief Remove all files in the transmit queue.
 * \param cmd Pointer to matching struct script_cmds script
 * command.
 \return SCRIPT_CODES error see \ref script_codes
 * \par Script Command:<br>
 * <tt>\"CLEAR TXQ:\"</tt><br>
 * \par Script Parameters:<br>
 * <tt>None</tt><br>
 *****************************************************************/
SCRIPT_CODES ScriptParsing::sc_hamlib(struct script_cmds *cmd)
{
	return script_no_errors;
}

/** **************************************************************
 * \brief Remove all files in the transmit queue.
 * \param cmd Pointer to matching struct script_cmds script
 * command.
 * \return SCRIPT_CODES error see \ref script_codes
 * \par Script Command:<br>
 * <tt>\"CLEAR TXQ:\"</tt><br>
 * \par Script Parameters:<br>
 * <tt>None</tt><br>
 *****************************************************************/
SCRIPT_CODES ScriptParsing::sc_rc_xmlrpc(struct script_cmds *cmd)
{
	return script_no_errors;
}

/** **************************************************************
 * \brief Remove all files in the transmit queue.
 * \param cmd Pointer to matching struct script_cmds script
 * command.
 * \return SCRIPT_CODES error see \ref script_codes
 * \par Script Command:<br>
 * <tt>\"CLEAR TXQ:\"</tt><br>
 * \par Script Parameters:<br>
 * <tt>None</tt><br>
 *****************************************************************/
SCRIPT_CODES ScriptParsing::sc_exec_macro(struct script_cmds *cmd)
{
	return script_no_errors;
}

/** **************************************************************
 * \brief Remove all files in the transmit queue.
 * \param cmd Pointer to matching struct script_cmds script
 * command.
 * \return SCRIPT_CODES error see \ref script_codes
 * \par Script Command:<br>
 * <tt>\"CLEAR TXQ:\"</tt><br>
 * \par Script Parameters:<br>
 * <tt>None</tt><br>
 *****************************************************************/
SCRIPT_CODES ScriptParsing::sc_hrdwr_ptt(struct script_cmds *cmd)
{
	return script_no_errors;
}

/*****************************************************************
 *****************************************************************
 CLASS SUPPORT ROUTINES
 *****************************************************************
 *****************************************************************/

/** **************************************************************
 * \brief Used for initialization of the function vector table.
 * \param cmd Pointer to matching struct script_cmds script
 * command.
 * \return SCRIPT_CODES error see \ref script_codes
 *****************************************************************/
SCRIPT_CODES ScriptParsing::sc_dummy(struct script_cmds *cmd)
{
	return script_no_errors;
}

/** **************************************************************
 * \brief Convert error numbers into human readable form.
 * \param error_no Error number to convert.
 * \param line_number The offending line number in the script file.
 * \param cmd The script command is question.
 * \return SCRIPT_CODES error see \ref script_codes
 *****************************************************************/
char * ScriptParsing::script_error_string(SCRIPT_CODES error_no, int line_number, char *cmd)
{
	char *es = (char *) "";

	memset(error_buffer,     0, sizeof(error_buffer));
	memset(error_cmd_buffer, 0, sizeof(error_cmd_buffer));
	memset(error_string,     0, sizeof(error_string));

	if(cmd) {
		strncpy(error_cmd_buffer, cmd, sizeof(error_cmd_buffer)-1);
	}

	switch(error_no) {
		case script_command_not_found:
			es =  (char *) _("Command Not Found");
			break;

		case script_non_script_file:
			es = (char *) _("Not a script file/tag not found");
			break;

		case script_parameter_error:
			es = (char *) _("Invalid parameter");
			break;

		case script_function_parameter_error:
			es = (char *) _("Invalid function parameter (internal non-script error)");
			break;

		case script_mismatched_quotes:
			es = (char *) _("Missing paired quotes (\")");
			break;

		case script_general_error:
			es = (char *) _("General Error");
			break;

		case script_no_errors:
			es = (char *) _("No Errors");
			break;

		case script_char_match_not_found:
			es = (char *) _("Character searched not found");
			break;

		case script_end_of_line_reached:
			es = (char *) _("End of line reached");
			break;

		case script_file_not_found:
			es = (char *) _("File not found");
			break;

		case script_path_not_found:
			es = (char *) _("Directory path not found");
			break;

		case script_args_eol:
			es = (char *) _("Unexpected end of parameter (args[]) list found");
			break;

		case script_param_check_eol:
			es = (char *) _("Unexpected end of parameter check list found");
			break;

		case script_paramter_exceeds_length:
			es = (char *) _("Character count in args[] parameter exceeds expectations");
			break;

		case script_memory_allocation_error:
			es = (char *) _("Memory Allocation Error (internal non-script error)");
			break;

		case script_incorrectly_assigned_value:
			es = (char *) _("Passed parameter is not of the expected type.");
			break;

		case script_invalid_parameter:
			es = (char *) _("Parameter is not valid.");
			break;

		case script_command_seperator_missing:
			es = (char *) _("Command missing ':'.");
			break;

		case script_max_sub_script_reached:
			es = (char *) _("Maximum open subscripts reached.");
			break;

		case script_subscript_exec_fail:
			es = (char *) _("Subscript execution fail (internal).");
			break;

		case script_device_path_not_found:
			es = (char *) _("Script device path not found.");
			break;

		default:
			es = (char *) _("Undefined error");
	}

	snprintf(error_buffer, sizeof(error_buffer)-1, _("Line: %d Error:%d %s (%s)"),
			 line_number, error_no, es, error_cmd_buffer);

	return error_buffer;
}

/** **************************************************************
 * \brief Search for first occurrence of a non white space
 * \param data Data pointer to search.
 * \param limit Number of bytes in the data buffer.
 * \param error returned error code.
 * \return Pointer to character if found. Otherwise, return null
 * \par Note:<br>
 * The searched condition is ignored if the expected content is
 * encapsulated in quotes \(\"\"\).
 *****************************************************************/
char * ScriptParsing::skip_white_spaces(char * data, char * limit, SCRIPT_CODES &error)
{
	char *cPtr      = (char *) 0;

	if(!data || !limit) {
		error = script_function_parameter_error;
		return (char *)0;
	}

	for(cPtr = data; cPtr < limit; cPtr++) {
		if(*cPtr > ' ') {
			error = script_no_errors;
			return cPtr;
		}
	}

	error = script_end_of_line_reached;


	return (char *)0;        // End of line reached.
}

/** **************************************************************
 * \brief Search for the first occurrence on a non number.
 * \param data Data pointer to search.
 * \param limit Number of bytes in the data buffer.
 * \param error returned error code.
 * \return Pointer to character if found. Otherwise, return null
 * \par Note:<br>
 * The searched condition is ignored if the expected content is
 * encapsulated in quotes \(\"\"\).
 *****************************************************************/
char * ScriptParsing::skip_numbers(char * data, char * limit, SCRIPT_CODES &error)
{
	char *cPtr  = (char *) 0;
	int  q_flag = 0;

	if(!data || !limit) {
		error = script_function_parameter_error;
		return (char *)0;
	}

	for(cPtr = data; cPtr < limit; cPtr++) {
		if(*cPtr == '"')     // Check for encapsulated strings ("")
			q_flag++;

		if((q_flag & 0x1))   // Continue if string is encapsulated
			continue;

		if(!isdigit(*cPtr)) {
			error = script_no_errors;
			return cPtr;
		}
	}

	if(q_flag & 0x1) {
		error = script_mismatched_quotes;
	} else {
		error = script_end_of_line_reached;
	}

	return (char *)0;        // End of line reached.
}

/** **************************************************************
 * \brief Skip characters until either a number or white space is
 * found.
 * \param data Data pointer to search.
 * \param limit Number of bytes in the data buffer.
 * \param error returned error code.
 * \return Pointer to character if found. Otherwise, return null
 * \par Note:<br>
 * The searched condition is ignored if the expected content is
 * encapsulated in quotes \(\"\"\).
 *****************************************************************/
char * ScriptParsing::skip_characters(char * data, char * limit, SCRIPT_CODES &error)
{
	char *cPtr  = (char *) 0;
	int  q_flag = 0;

	if(!data || !limit) {
		error = script_function_parameter_error;
		return (char *)0;
	}

	for(cPtr = data; cPtr < limit; cPtr++) {
		if(*cPtr == '"')     // Check for encapsulated strings ("")
			q_flag++;

		if((q_flag & 0x1))   // Continue if string is encapsulated
			continue;

		if(isdigit(*cPtr) || *cPtr <= ' ') {
			error = script_no_errors;
			return cPtr;
		}
	}

	if(q_flag & 0x1) {
		error = script_mismatched_quotes;
	} else {
		error = script_end_of_line_reached;
	}

	return (char *)0;        // End of line reached.
}

/** **************************************************************
 * \brief Search for the first occurrence of a white space.
 * \param data Data pointer to search.
 * \param limit Number of bytes in the data buffer.
 * \param error returned error code.
 * \return Pointer to character if found. Otherwise, return null
 * \par Note:<br>
 * The searched condition is ignored if the expected content is
 * encapsulated in quotes \(\"\"\).
 *****************************************************************/
char * ScriptParsing::skip_alpha_numbers(char * data, char * limit, SCRIPT_CODES &error)
{
	char *cPtr  = (char *) 0;
	int  q_flag = 0;

	if(!data || !limit) {
		error = script_function_parameter_error;
		return (char *)0;
	}

	for(cPtr = data; cPtr < limit; cPtr++) {

		if(*cPtr == '"')     // Check for encapsulated strings ("")
			q_flag++;

		if((q_flag & 0x1))   // Continue if string is encapsulated
			continue;

		if(*cPtr <= ' ') {
			error = script_no_errors;
			return cPtr;
		}
	}

	if(q_flag & 0x1) {
		error = script_mismatched_quotes;
	} else {
		error = script_end_of_line_reached;
	}

	return (char *)0;        // End of line reached.
}

/** **************************************************************
 * \brief Search for first occurrence of 'character'
 * \param c Character to search for
 * \param data Pointer to Data to search for character in.
 * \param limit Number of bytes in the data buffer.
 * \param error returned error code.
 * \return Pointer to character if found. Otherwise, return null
 * \par Note:<br>
 * The searched condition is ignored if the expected content is
 * encapsulated in quotes \(\"\"\).
 *****************************************************************/
char * ScriptParsing::skip_to_character(char c, char * data, char * limit, SCRIPT_CODES &error)
{
	char *cPtr  = (char *) 0;
	int  q_flag = 0;

	if(!data || !limit) {
		error = script_function_parameter_error;
		return (char *)0;
	}

	for(cPtr = data; cPtr < limit; cPtr++) {
		if(*cPtr == '"')     // Check for encapsulated strings ("")
			q_flag++;

		if((q_flag & 0x1))   // Continue if string is encapsulated
			continue;

		if(*cPtr == c)   {    // Match found. Return pointer to it's location
			error = script_no_errors;
			return cPtr;
		}
	}

	if(q_flag & 0x1) {
		error = script_mismatched_quotes;
	} else {
		error = script_end_of_line_reached;
	}

	return (char *)0;        // End of line reached.
}

/** **************************************************************
 * \brief Replace CR, LF, and '#' with '0' (by value)
 * \param data Search data pointer
 * \param limit data buffer size
 * \return void (none)
 * \par Note:<br>
 * The searched condition is ignored if the remark character \(#\)
 * is encapsulated in quotes \(\"\"\).
 *****************************************************************/
SCRIPT_CODES ScriptParsing::remove_crlf_comments(char *data, char *limit, size_t &count)
{
	char *cPtr  = (char *) 0;
	int  q_flag = 0;

	SCRIPT_CODES error = script_no_errors;

	if(!data || !limit)
		return script_function_parameter_error;

	count = 0;

	for(cPtr = data; cPtr < limit; cPtr++) {
		if(*cPtr == '\r' || *cPtr == '\n') {
			*cPtr = 0;
			return script_no_errors;
		}

		if(*cPtr == '"')
			q_flag++;

		if((q_flag & 0x1))
			continue;

		if(*cPtr == '#') {
			*cPtr = 0;
			break;
		}

		if(*cPtr > ' ')
			count++;

	}

	// Remove trailing white spaces.
	while(cPtr >= data) {
		if(*cPtr <= ' ') *cPtr = 0;
		else break;
		cPtr--;
	}

	if(q_flag & 0x1) {
		error = script_mismatched_quotes;
	} else {
		error = script_end_of_line_reached;
	}

	return error;
}

/** **************************************************************
 * \brief Copy memory from address to address to the source buffer.
 * \param buffer Destination buffer
 * \param sPtr Start of the copy Address
 * \param ePtr End of the copy Address
 * \param limit Destination buffer size
 * command.
 * \return SCRIPT_CODES error see \ref script_codes
 *****************************************************************/
SCRIPT_CODES ScriptParsing::copy_string_uppercase(char *buffer, char *sPtr, char *ePtr, size_t limit)
{
	if(!buffer || !sPtr || !ePtr || limit < 1) {
		return script_function_parameter_error;
	}

	char *dPtr   = buffer;
	size_t index = 0;

	for(index = 0; index < limit; index++) {
		*dPtr++ = toupper(*sPtr++);
		if(sPtr >= ePtr) break;
	}

	return script_no_errors;
}

/** **************************************************************
 * \brief Parse the parameters and seperate into individual components.
 * \param s char pointer to the start of the string.
 * \param e char pointer to the end of the string.
 * \param matching_command pointer to the data strucure of the matching
 * command. See \ref SCRIPT_COMMANDS
 * \return SCRIPT_CODES error see \ref script_codes
 *****************************************************************/
SCRIPT_CODES ScriptParsing::parse_parameters(char *s, char *e, SCRIPT_COMMANDS *matching_command)
{
	char *c   = s;
	char *d   = (char *)0;
	int index = 0;
	int parameter_count = matching_command->argc;
	int count = 0;
	long tmp  = 0;

	SCRIPT_CODES error = script_no_errors;

	// Clear the old pointers.
	for(index = 0; index < MAX_CMD_PARAMETERS; index++) {
		matching_command->args[index] = (char *)0;
	}

	if(parameter_count > 0) {
		count = parameter_count - 1;
		for(index = 0; index < count; index++) {
			c = skip_white_spaces(c, e, error);

			if(error != script_no_errors)
				return script_parameter_error;

			d = skip_to_character(',', c, e, error);

			if(error != script_no_errors)
				return script_parameter_error;

			*d = 0;
			tmp = (long) (d - c);
			if(tmp > 0)
				trim(c, (size_t)(tmp));
			matching_command->args[index] = c;
			c = d + 1;
		}

		c = skip_white_spaces(c, e, error);
		if(error) return error;

		d = skip_alpha_numbers(c, e, error);
		if(error) return error;

		*d = 0;
		tmp = (long) (d - c);
		if(tmp > 0)
			trim(c, (size_t)(tmp));

		matching_command->args[index] = c;
	}

#ifdef TESTING
	for(int i = 0; i < parameter_count;  i++)
		if(matching_command->args[i])
			printf("parameters %d (%s)\n", i, matching_command->args[i]);
#endif

	error = check_parameters(matching_command);

	if(error != script_no_errors)
		return error;

	if(matching_command->func)
		error = (this->*matching_command->func)(matching_command);
	if(error) return error;


	return script_no_errors;
}

/** **************************************************************
 * \brief Execute callback function.
 * \param cb_data Pointer for making a copy of the data to prevent
 * exterior alteration of source information.
 * \return 0 = No error<br> \< 0 = Error<br>
 *****************************************************************/
int ScriptParsing::call_callback(SCRIPT_COMMANDS *cb_data)
{
	int argc     = 0;
	int error    = 0;
	int index    = 0;
	SCRIPT_COMMANDS *tmp = (SCRIPT_COMMANDS *)0;
	size_t count = 0;

	if(!cb_data || !cb_data->cb) return -1;

	argc = cb_data->argc;

	tmp = new SCRIPT_COMMANDS;

	if(!tmp) return -1;

	memset(tmp, 0, sizeof(SCRIPT_COMMANDS));

	for(index = 0; index < argc; index++) {
		if(cb_data->args[index]) {
			count = strnlen(cb_data->args[index], MAX_PARAMETER_LENGTH-1);
			tmp->args[index] = new char[count+1];
			if(tmp->args[index]) {
				memset(tmp->args[index], 0, count+1);
				strncpy(tmp->args[index], cb_data->args[index], count);
			} else {
				error = -1;
				break;
			}
		} else break;
	}

	if(error > -1) {
		// Fill SCRIPT_COMMANDS (tmp) struct with useful data.
		tmp->flags          = cb_data->flags;
		tmp->command_length = cb_data->command_length;
		tmp->argc           = cb_data->argc;
		strncpy(tmp->command, cb_data->command, MAX_COMMAND_LENGTH);

		// Initialize with do nothing functions
		tmp->func = &ScriptParsing::sc_dummy;
		tmp->cb   = callback_dummy;

		error = (*cb_data->cb)(this, tmp);
	}

	if(tmp) {
		for(index = 0; index < argc; index++) {
			if(tmp->args[index]) {
				delete [] tmp->args[index];
			}
		}

		delete tmp;
	}

	return error;
}

/** **************************************************************
 * \brief Parse a single line of data from the script file being read.
 * \param data Pointer the the script scring in question
 * \param buffer_size buffer size of the data pointer.
 * command.
 * \return SCRIPT_CODES error see \ref script_codes
 *****************************************************************/
SCRIPT_CODES ScriptParsing::parse_hierarchy_command(char *data, size_t buffer_size)
{
	char *buffer = (char *)0;
	char *cPtr   = (char *)0;
	char *endPtr = (char *)0;
	char *ePtr   = (char *)0;
	int allocated_buffer_size = MAX_COMMAND_LENGTH;
	int callback_error = 0;
	size_t cmd_size    = 0;
	size_t cmp_results = 0;
	size_t size        = 0;

	SCRIPT_CODES error = script_no_errors;
	SCRIPT_COMMANDS *local_list = _script_commands;
	size_t local_limit = _script_command_count;

	cPtr = data;
	endPtr = &data[buffer_size];

	cPtr = skip_white_spaces(cPtr, endPtr, error);
	if(error != script_no_errors) return error;

	ePtr = skip_to_character(':', cPtr, endPtr, error);
	if(error != script_no_errors) return script_command_seperator_missing;

	buffer = new char [allocated_buffer_size];
	if(!buffer) {
		LOG_INFO(_("Buffer allocation Error near File: %s Line %d"), __FILE__, __LINE__);
		return script_memory_allocation_error;
	}

	memset(buffer, 0, allocated_buffer_size);
	error = copy_string_uppercase(buffer, cPtr, ePtr, allocated_buffer_size-1);

	if(error != script_no_errors) {
		buffer[0] = 0;
		delete [] buffer;
		return error;
	}

	int str_count = str_cnt(buffer, allocated_buffer_size);
	trim(buffer, (size_t) str_count);

	char sub_command[MAX_COMMAND_LENGTH];
	int sub_length = 0;
	bool not_found = true;
	char *endCmd = ePtr;
	char *endCmdPtr = endPtr;

	cPtr = buffer;
	endPtr = &buffer[str_count];

	while(not_found) {
		memset(sub_command, 0, sizeof(sub_command));

		ePtr = skip_to_character('.', cPtr, endPtr, error);

		if(error == script_end_of_line_reached) {
			ePtr = endPtr;
			error = script_no_errors;
		}

		if(error == script_no_errors) {
			copy_string_uppercase(sub_command, cPtr, ePtr, MAX_COMMAND_LENGTH-1);
			cPtr = ePtr + 1;
		}
		else
			break;

		for(int i = 0; i < local_limit; i++) {

			if(local_list[i].command[0] == 0) {
				not_found = false;
				error = script_command_not_found;
				break;
			}

			if(strncmp(sub_command, local_list[i].command, MAX_COMMAND_LENGTH) == 0) {

				if((local_list[i].param_check[0] == p_list) && local_list[i].sub_commands) {
					local_limit = local_list[i].sub_command_count; // This must be first before assigning local_list
					local_list  = local_list[i].sub_commands;
					break;
				}

				if(file_type() & local_list[i].flags) {
					error = parse_parameters(++endCmd, endCmdPtr, &local_list[i]);

					if(error)  {
						buffer[0] = 0;
						delete [] buffer;
						return error;
					}

					if(local_list[i].cb) {
						error = (SCRIPT_CODES) call_callback(&local_list[i]);
						if(error < script_no_errors)
							LOG_INFO(_("Call back for script command %s reported an Error"), local_list[local_limit].command);
						not_found = false;
						error = script_command_handled;
						break;
					}
				} else {
					LOG_INFO(_("Command %s ignored, not supported in current file type:"), buffer);
					if(file_type() & SCRIPT_COMMAND) LOG_INFO("Script.");
					not_found = false;
					break;
				}
			}
		}
	}

	buffer[0] = 0;
	delete [] buffer;

	return error;
}

/** **************************************************************
 * \brief Parse a single line of data from the script file being read.
 * \param data Pointer the the script scring in question
 * \param buffer_size buffer size of the data pointer.
 * command.
 * \return SCRIPT_CODES error see \ref script_codes
 *****************************************************************/
SCRIPT_CODES ScriptParsing::parse_single_command(SCRIPT_COMMANDS **cur_list, size_t *limit, char *data, size_t buffer_size)
{
	char *buffer = (char *)0;
	char *cPtr   = (char *)0;
	char *endPtr = (char *)0;
	char *ePtr   = (char *)0;
	int allocated_buffer_size = MAX_COMMAND_LENGTH;
	int callback_error = 0;
	size_t cmd_size    = 0;
	size_t cmp_results = 0;
	size_t index       = 0;
	size_t size        = 0;

	SCRIPT_CODES error = script_no_errors;
	SCRIPT_COMMANDS *local_list = *cur_list;
	size_t local_limit = *limit;

	cPtr = data;
	endPtr = &data[buffer_size];

	cPtr = skip_white_spaces(cPtr, endPtr, error);
	if(error != script_no_errors) return error;

	ePtr = skip_to_character(':', cPtr, endPtr, error);
	if(error != script_no_errors) return script_command_seperator_missing;

	buffer = new char [allocated_buffer_size];
	if(!buffer) {
		LOG_INFO(_("Buffer allocation Error near File: %s Line %d"), __FILE__, __LINE__);
		return script_memory_allocation_error;
	}

	memset(buffer, 0, allocated_buffer_size);
	error = copy_string_uppercase(buffer, cPtr, ePtr, allocated_buffer_size-1);
	if(error != script_no_errors) {
		buffer[0] = 0;
		delete [] buffer;
		return error;
	}

	int str_count = str_cnt(buffer, allocated_buffer_size);
	trim(buffer, (size_t) str_count);


	for(int i = 0; i < str_count; i++) {
		if(buffer[i] == '.')
			error = parse_hierarchy_command(data, buffer_size);
	}

	if(error != script_command_handled) {
		for(index = 0; index < local_limit; index++) {
			size = strnlen(local_list[index].command, MAX_COMMAND_LENGTH);
			cmd_size = strnlen(buffer, MAX_COMMAND_LENGTH);
			cmp_results = memcmp(buffer, local_list[index].command, size);

			if(cmp_results == 0 && (cmd_size == size)) {

				if(local_list[index].param_check[0] == p_list_end) {
					return script_end_of_list_reached;
				}

				if(local_list[index].sub_commands && local_list[index].param_check[0] == p_list) {
					*cur_list = local_list[index].sub_commands;
					*limit    = local_list[index].sub_command_count;
					return script_list;
				}

				if(file_type() & local_list[index].flags) {
					error = parse_parameters(++ePtr, endPtr, &local_list[index]);
					if(error)  {
						buffer[0] = 0;
						delete [] buffer;
						return error;
					}

					if(local_list[index].cb) {
						error = (SCRIPT_CODES) call_callback(&local_list[index]);
						if(error < script_no_errors)
							LOG_INFO(_("Call back for script command %s reported an Error"), local_list[local_limit].command);
					}
				} else {
					LOG_INFO(_("Command %s ignored, not supported in current file type:"), buffer);
					if(file_type() & SCRIPT_COMMAND) LOG_INFO("Script.");
				}
				break;
			}
		}
	}
	buffer[0] = 0;
	delete [] buffer;

	return error;
}

/** **************************************************************
 * \brief Script entry point for parsing the script file.
 * \param file_name_path path and file name for the script to parse.
 * \return SCRIPT_CODES error see \ref script_codes
 *****************************************************************/
SCRIPT_CODES ScriptParsing::read_file(FILE *fd, SCRIPT_COMMANDS *cur_list, size_t limit, int *line_number)
{
	SCRIPT_CODES return_code = script_no_errors;
	SCRIPT_COMMANDS *local_list = cur_list;
	size_t local_limit = limit;
	size_t count    = 0;
	bool errors_reported = false;

	recursive_count++;

	if(recursive_count >= RECURSIVE_LIMIT)
		return script_recursive_limit_reached;

	while(1) {
		if(ferror(fd) || feof(fd)) break;

		memset(line_buffer, 0, sizeof(line_buffer));
		(*line_number)++;
		fgets(line_buffer, sizeof(line_buffer) - 1, fd);

#ifdef TESTING
		printf("Reading: %s", line_buffer);
#endif

		return_code = remove_crlf_comments(line_buffer, &line_buffer[sizeof(line_buffer)], count);

		if(count < 1) {
			continue;
		}

		if(return_code >= script_no_errors) {
			return_code = parse_single_command(&local_list, &limit, line_buffer, sizeof(line_buffer) - 1);

			if(return_code == script_list && recursive_count < RECURSIVE_LIMIT) {
				read_file(fd, local_list, local_limit, line_number);
			}

			if(return_code == script_end_of_list_reached)
				break;
		}

		if(return_code < script_no_errors) {
			LOG_INFO("%s", script_error_string(return_code, *line_number, line_buffer));
			errors_reported = true;
		}
	}

	recursive_count--;

	if(errors_reported)
		return script_errors_reported;

	return script_no_errors;
}

/** **************************************************************
 * \brief Script entry point for parsing the script file.
 * \param file_name_path path and file name for the script to parse.
 * \return SCRIPT_CODES error see \ref script_codes
 *****************************************************************/
SCRIPT_CODES ScriptParsing::parse_commands(char *file_name_path)
{
	char *cPtr      = (char *)0;
	FILE *fd        = (FILE *)0;
	int line_number = 0;
	SCRIPT_CODES error_code = script_no_errors;
	size_t tmp      = 0;
	SCRIPT_COMMANDS *local_script_commands = _script_commands;
	size_t local_script_command_count      = _script_command_count;

	if(!file_name_path) {
		LOG_INFO(_("Invalid function parameter 'char *file_name_path' (null)"));
		return script_general_error;
	}

	fd = fopen(file_name_path, "r");

	if(!fd) {
		LOG_INFO(_("Unable to open file %s"), file_name_path);
		return script_general_error;
	}

	memset(line_buffer, 0, sizeof(line_buffer));

	line_number++;
	char *retval = fgets(line_buffer, sizeof(line_buffer) - 1, fd);

	tmp = strlen(SCRIPT_FILE_TAG);
	line_buffer[tmp] = 0;
	tmp = strncmp(SCRIPT_FILE_TAG, line_buffer, tmp);

	if(!retval || tmp) {
		cPtr = script_error_string(script_non_script_file, line_number, line_buffer);
		LOG_INFO("%s", cPtr);
		fclose(fd);
		return script_non_script_file;
	}

	error_code = read_file(fd, local_script_commands, local_script_command_count, &line_number);

	fclose(fd);

	return error_code;
}
/** **************************************************************
 * \brief Assign a list of valid parameters for verification checks.
 * \param array An Array of pointers to each element.
 * \param array_count Number of entries in the array.
 * \return the array count or '0' if error.
 * \par Note:
 * This array is limited to the first parameter of the command
 * used in it's comparison.
 *****************************************************************/
int ScriptParsing::assign_valid_parameters(const char *command, const char **array, const int array_count)
{
	if(!array || array_count < 1 || !command) return 0;

	int index = 0;
	int count = 0;

	SCRIPT_COMMANDS * cmd_sc = search_command(_script_commands, 0, command);

	if(!cmd_sc) {
		return 0;
	}

	for(index = 0; index < array_count; index++) {
		if(*array[index]) count++;
	}

	if(count != array_count) return 0;

	cmd_sc->valid_values = array;
	cmd_sc->valid_value_count = array_count;

	return array_count;
}

/** **************************************************************
 * \brief Return true state if string is matched.
 * \param state Referenced value to assign results to.
 * \param string Pointer to the data string.
 * \param true_state Pointer to the data to match with.
 * \return SCRIPT_CODES error code.
 *****************************************************************/
inline SCRIPT_CODES ScriptParsing::test_on_off_state(bool &state, char *string, char *true_state=(char *)"ON")
{
	if(!string || !true_state) {
		return script_function_parameter_error;
	}

	bool flag = false;

	if(strncmp(string, true_state, MAX_PARAMETER_LENGTH) == 0)
		flag = true;

	state = flag;

	return script_no_errors;
}

/** **************************************************************
 * \brief Validate if file is located in the specified location.
 * \param filename Pointer to a series of charcters
 * \return SCRIPT_CODES error code.
 *****************************************************************/
SCRIPT_CODES ScriptParsing::check_filename(char *filename, char *full_name_path=0, size_t limit=0)
{
	char *filename_path = (char *)0;
	char *path = (char *)0;
	SCRIPT_CODES error = script_no_errors;
	std::string user_path = "";

	if(!filename) {
		return script_function_parameter_error;
	}

	filename_path = new char[FILENAME_MAX + 1];

	if(!filename_path) {
		return script_memory_allocation_error;
	}

	memset(filename_path, 0, (FILENAME_MAX + 1));

	path = new char[FILENAME_MAX + 1];

	if(!path) {
		delete [] filename_path;
		return script_memory_allocation_error;
	}

	memset(path, 0, (FILENAME_MAX + 1));

	user_path.assign(this->path());

	if(user_path.empty()) {
		get_current_dir(path, FILENAME_MAX);
	} else {
		strncpy(path, user_path.c_str(), FILENAME_MAX);
	}

	size_t size = strnlen(path, FILENAME_MAX);

	if(size > 1) {
		if(path[size - 1] != PATH_CHAR_SEPERATOR) {
			strncat(path, PATH_SEPERATOR, FILENAME_MAX);
		}
	}

	strncpy(filename_path, path, FILENAME_MAX);
	strncat(filename_path, filename, FILENAME_MAX);

#ifdef TESTING
	printf("     filename = %s\n", filename);
	printf("         path = %s\n", path);
	printf("filename_path = %s\n", filename_path);
#endif

	FILE *fd = (FILE *)0;

	fd = fopen(filename_path, "r");

	if(!fd) {
		error = script_file_not_found;
	} else {
		fclose(fd);

		if(full_name_path && limit > 0)
			strncpy(full_name_path, filename_path, limit);
	}

	delete [] filename_path;
	delete [] path;

	return error;
}

/** **************************************************************
 * \brief Validate if path is present.
 * \param path The path to verify.
 * \return SCRIPT_CODES error code.
 *****************************************************************/
SCRIPT_CODES ScriptParsing::check_path(const char *path)
{
	if(!path) {
		return script_function_parameter_error;
	}

	struct stat st;
	memset(&st, 0, sizeof(struct stat));

	if(stat(path, &st) == 0) {
		if(st.st_mode & S_IFDIR)
			return script_no_errors;
	}

	return script_path_not_found;
}

/** **************************************************************
 * \brief Validate if device path is present.
 * \param path The path to verify.
 * \return SCRIPT_CODES error code.
 *****************************************************************/
SCRIPT_CODES ScriptParsing::check_dev_path(const char *path)
{
	if(!path) {
		return script_function_parameter_error;
	}

	struct stat st;
	memset(&st, 0, sizeof(struct stat));

	if(stat(path, &st) == 0) {
		if(!(st.st_mode & S_IFDIR))
			return script_no_errors;
	}

	return script_device_path_not_found;
}

/** **************************************************************
 * \brief Validte bool representation.
 * \param path The path to verify.
 * \return SCRIPT_CODES error code.
 *****************************************************************/
SCRIPT_CODES ScriptParsing::check_bool(const char *value, bool &flag)
{
	if(!value) {
		return script_function_parameter_error;
	}

	static char *bool_flags[] = {
		(char *)_("ENABLE"), (char *)_("DISABLE"),
		(char *)_("YES"), (char *)_("NO"), (char *)_("ON"), (char *)_("OFF"),
		(char *)_("0"), (char *)_("1"), (char *)0
	};

	for(int i = 0; i < sizeof(bool_flags)/sizeof(char *); i++) {
		if(bool_flags[i] == (char *)0) break;
		if(strncmp(value, bool_flags[i], 7) == 0) {
			flag = true;
			return script_no_errors;
		}
	}

	flag = false;
	return script_invalid_parameter;
}

/** **************************************************************
 * \brief Validate if the parameter is a value.
 * \param value The string in question.
 * \param p format verification.
 * \return SCRIPT_CODES error code.
 *****************************************************************/
SCRIPT_CODES ScriptParsing::check_numbers(char *value, paramter_types p)
{
	SCRIPT_CODES error = script_no_errors;
	size_t length = 0;
	size_t index = 0;
	int data_count = 0;
	int signed_value = 0;
	int decimal_point = 0;

	if(!value)
		return script_function_parameter_error;

	length = strnlen(value, MAX_PARAMETER_LENGTH);

	if(length < 1)
		return script_parameter_error;

	// Skip any leading white spaces.
	for(index = 0; index < length; index++) {
		if(value[index] > ' ')
			break;
	}

	if((index >= length))
		return script_parameter_error;

	switch(p) {
		case p_int:
		case p_long:

			if(value[0] == '-' || value[0] == '+') {
				index++;
				signed_value++;
			}

		case p_unsigned_int:
		case p_unsigned_long:

			for(; index< length; index++) {
				if(isdigit(value[index]))
					data_count++;
				else
					break;
			}
			break;

			if(data_count)
				return script_no_errors;

		case p_float:
		case p_double:
			if(value[0] == '-' || value[0] == '+') {
				index++;
				signed_value++;
			}

			for(; index< length; index++) {
				if(isdigit(value[index]))
					data_count++;

				if(value[index] == '.')
					decimal_point++;

				if(decimal_point > 1)
					return script_parameter_error;
			}

			if(data_count)
				return script_no_errors;

			break;

		default:;

	}

	return error;
}

/** **************************************************************
 * \brief Validate the script parameter(s) are of the expected format.
 * \param cmd Matching command data structure.
 * \param p A table of expected parameters types (null terminated).
 * \param p_count the number of 'p[]' items in the table (includes null termination).
 * \return SCRIPT_CODES error code.
 *****************************************************************/
SCRIPT_CODES ScriptParsing::check_parameters(struct script_cmds *cmd)
{
	SCRIPT_CODES error = script_no_errors;
	int count   = 0;
	int index   = 0;
	size_t size = 0;
	bool flag = false;

	if(!cmd)
		return script_function_parameter_error;

	count = cmd->argc;

	if(count < 1)
		return script_no_errors;

	for(index = 0; index < count; index++) {

		if(!cmd->args[index]) {
			return script_args_eol;
		}

		if(cmd->param_check[index] == p_null) {
			size = 0;
		} else {
			size = strnlen(cmd->args[index], MAX_COMMAND_LENGTH);
		}

		switch(cmd->param_check[index]) {
			case p_null:
				error = script_param_check_eol;
				break;

			case p_char:
				if(size > 1)
					error = script_paramter_exceeds_length;
				break;

			case p_bool:
				error = check_bool(cmd->args[index], flag);
				break;

			case p_int:
			case p_long:
			case p_unsigned_int:
			case p_unsigned_long:
			case p_float:
			case p_double:
				error = check_numbers(cmd->args[index], cmd->param_check[index]);
				break;

			case p_dev_path:
				error = check_dev_path(cmd->args[index]);
				break;

			case p_dev_name:
			case p_string:
				if(size < 1)
					error = script_parameter_error;
				break;

			case p_path:
				error = check_path(cmd->args[index]);
				break;

			case p_filename:
				error = check_filename(cmd->args[index]);
				break;

			case p_list:
			case p_list_end:
			case p_void:
				break;
		}

		if(error != script_no_errors)
			break;
	}

	return error;
}

/** **************************************************************
 * \brief Search the content of SCRIPT_COMMANDS structure table
 * for the specified command.
 * \param command The command to search for.
 * \return Pointer to the matching SCRIPT_COMMANDS entry. Null if
 * not found.
 *****************************************************************/
SCRIPT_COMMANDS * ScriptParsing::search_command(SCRIPT_COMMANDS * table, size_t limit, const char *command)
{
	char *cmd_buffer = (char *)0;
	int diff = 0;
	SCRIPT_COMMANDS * found = (SCRIPT_COMMANDS *) 0;
	size_t count = limit;
	size_t index = 0;

	if(!command) return found;

	cmd_buffer = new char [MAX_COMMAND_LENGTH];

	if(!cmd_buffer) {
		LOG_INFO(_("cmd_buffer allocation error near line %d"), __LINE__);
		return found;
	}

	memset(cmd_buffer, 0, MAX_COMMAND_LENGTH);
	strncpy(cmd_buffer, command, MAX_COMMAND_LENGTH-1);

	to_uppercase(cmd_buffer, (int) MAX_COMMAND_LENGTH);
	trim(cmd_buffer, (size_t) MAX_COMMAND_LENGTH);

	for(index = 0; index < count; index++) {
		diff = strncmp(cmd_buffer, table[index].command, MAX_COMMAND_LENGTH);
		if(diff == 0) {
			found = &table[index];
			break;
		}
	}

	cmd_buffer[0] = 0;
	delete [] cmd_buffer;

	return found;
}

/** **************************************************************
 * \brief Convert string to uppercase characters.<br>
 * \par str Pointer to data.
 * \par limit data buffer size
 * \return void
 *****************************************************************/
void ScriptParsing::to_uppercase(char *str, int limit)
{
	if(!str || limit < 1) return;

	int character = 0;
	int count     = 0;
	int index     = 0;

	count = (int) strnlen(str, limit);

	for(index = 0; index < count; index++) {
		character = str[index];
		if(character == 0) break;
		character = (char) toupper(character);
		str[index] = character;
	}
}

/** **************************************************************
 * \brief Convert string to uppercase characters.<br>
 * \par str String storage passed by reference.
 * \return void
 *****************************************************************/
void ScriptParsing::to_uppercase(std::string &str)
{
	int character = 0;
	int count     = 0;
	int index     = 0;

	count = (int) str.length();

	for(index = 0; index < count; index++) {
		character = str[index];
		if(character == 0) break;
		character = (char) toupper(character);
		str[index] = character;
	}
}

/** **************************************************************
 * \brief Assign Call back function to a given script command.<br>
 * \param scriptCommand Script command string<br>
 * \param cb Pointer to call back function. int (*cb)(ScriptParsing *sp, SCRIPT_COMMANDS *sc)
 *****************************************************************/
int ScriptParsing::assign_callback(const char *scriptCommand, int (*cb)(ScriptParsing *sp, SCRIPT_COMMANDS *sc))
{
	char *cmd_buffer = (char *)0;
	int diff = 0;
	size_t count = _script_command_count;
	size_t index = 0;

	if(!scriptCommand || !cb) return 0;

	cmd_buffer = new char[MAX_COMMAND_LENGTH];

	if(!cmd_buffer) {
		LOG_INFO(_("cmd_buffer allocation error near line %d"), __LINE__);
		return 0;
	}

	memset(cmd_buffer, 0, MAX_COMMAND_LENGTH);
	strncpy(cmd_buffer, scriptCommand, MAX_COMMAND_LENGTH-1);

	to_uppercase(cmd_buffer, (int) MAX_COMMAND_LENGTH);
	trim(cmd_buffer, (size_t) MAX_COMMAND_LENGTH);

	for(index = 0; index < count; index++) {
		diff = strncmp(cmd_buffer, _script_commands[index].command, MAX_COMMAND_LENGTH);
		if(diff == 0) {
			if(_script_commands[index].cb)
				LOG_INFO(_("Over writing call back funcion for \"%s\""), cmd_buffer);
			_script_commands[index].cb = cb;
			break;
		}
	}

	cmd_buffer[0] = 0;
	delete [] cmd_buffer;

	return 0;
}

/** **************************************************************
 * \brief Assign member calling function to a command
 * \param top_command Top Command
 * \param sub_command Sub command
 * \param func Assigned calling function member
 * \return void (nothing)
 *****************************************************************/
void ScriptParsing::assign_member_func(char *top_command, char *sub_command, SCRIPT_CODES (ScriptParsing::*func)(struct script_cmds *))
{
	if(!top_command || !func) return;

	SCRIPT_COMMANDS *top_table = (SCRIPT_COMMANDS *)0;
	SCRIPT_COMMANDS *modify_table = (SCRIPT_COMMANDS *)0;

	if(top_cache) {
		if(strncmp(top_cache->command, top_command, MAX_COMMAND_LENGTH) == 0) {
			top_table = top_cache;
		}
	}

	if(!top_table) {
		for(int i = 0; i < _script_command_count; i++) {
			if(strncmp(_script_commands[i].command, top_command, MAX_COMMAND_LENGTH) == 0) {
				top_cache = &_script_commands[i];
				top_table = top_cache;
				break;
			}
		}
	}

	if(!top_table)
		return;

	if(!sub_command)
		modify_table = top_table;

	if(modify_table == (SCRIPT_COMMANDS *)0) {
		if(top_table->sub_commands) {
			for(int i = 0; i < top_table->sub_command_count; i++) {
				if(strncmp(sub_command, top_table->sub_commands[i].command, MAX_COMMAND_LENGTH) == 0) {
					modify_table = &top_table->sub_commands[i];
					break;
				}
			}
		}
	}

	if(modify_table != (SCRIPT_COMMANDS *)0) {
		modify_table->func = func;
		modify_table->command_length = strnlen(modify_table->command, MAX_COMMAND_LENGTH);
	}
}

/** **************************************************************
 * \brief Initialize variables
 * \return void (nothing)
 *****************************************************************/
void ScriptParsing::clear_script_parameters(bool all)
{
	if(all) {
		_callsign_info.clear();
		_name_info.clear();
		_qth_info.clear();
		_locator_info.clear();
		_antenna_info.clear();
	}

	_use_oss_audio_device = false;
	_oss_audio_device_path.clear();

	_use_port_audio_device = false;
	_capture_path.clear();
	_playback_path.clear();

	_use_pulse_audio_device = false;
	_pulse_audio_device_path.clear();

	_audio_device_sample_rate_capture.clear();
	_audio_device_sample_rate_playback.clear();
	_audio_device_converter.clear();

	_rx_ppm = 0;
	_tx_ppm = 0;
	_tx_offset = 0;

	_mono_audio_output = false;
	_modem_signal_left_right = false;
	_reverse_left_right = false;

	_ptt_tone_right_channel = false;
	_cw_qsk_right_channel = false;
	_pseudo_fsk_right_channel = false;

	_wave_file_sample_rate = 0;

	_hrdw_ptt_right_audio_channel = false;
	_hrdw_ptt_sep_serial_port = false;
	_hrdw_ptt_sep_serial_port_path.clear();
	_hrdw_ptt_sep_serial_port_rts = false;
	_hrdw_ptt_sep_serial_port_dtr = false;
	_hrdw_ptt_sep_serial_port_rts_v = false;
	_hrdw_ptt_sep_serial_port_dtr_v = false;
	_hrdw_ptt_start_delay = 0;
	_hrdw_ptt_end_delay = 0;
	_hrdw_ptt_initialize = false;

	_use_rigcat = false;
	_rigcat_desc_file.clear();
	_rigcat_device_path.clear();
	_rigcat_retries = 0;
	_rigcat_retry_interval = 0;
	_rigcat_write_delay = 0;
	_rigcat_init_delay = 0;
	_rigcat_baud_rate.clear();
	_rigcat_stop_bits.clear();
	_rigcat_commands_echoed = false;
	_rigcat_toggle_rts_ptt = false;
	_rigcat_restore_on_close = false;
	_rigcat_cat_command_ptt = false;
	_rigcat_dtr_12v = false;
	_rigcat_vsp_enable = false;
	_rigcat_initialize = false;

	_use_hamlib = false;
	_hamlib_rig.clear();
	_hamlib_retries = 0;
	_hamlib_retry_interval = 0;
	_hamlib_write_delay = 0;
	_hamlib_post_write_delay = 0;
	_hamlib_device_path.clear();
	_hamlib_baud_rate.clear();
	_hamlib_stop_bits.clear();
	_hamlib_sideband.clear();
	_hamlib_ptt_hl_command = false;
	_hamlib_dtr_12 = false;
	_hamlib_rts_12 = false;
	_hamlib_rts_cts_flow = false;
	_hamlib_xon_xoff_flow = false;
	_hamlib_advanced_config.clear();
	_hamlib_initialize = false;

	_use_xml_rpc = false;
	_xml_rpc_mode_bw_delay.clear();
	_xml_rpc_initialize = false;

	_path.clear();
	_file_type = SCRIPT_COMMAND;
	_macro_command.clear();

}


/** **************************************************************
 * \brief Initialize callable members.
 * \return void (nothing)
 *****************************************************************/
void ScriptParsing::initialize_function_members(void)
{
	assign_member_func((char *) CMD_OPERATOR,         (char *) 0, &ScriptParsing::sc_operator);
	assign_member_func((char *) CMD_AUDIO_DEVICE,     (char *) 0, &ScriptParsing::sc_audio_device);
	assign_member_func((char *) CMD_AUDIO_SETTINGS,   (char *) 0, &ScriptParsing::sc_audio_settings);
	assign_member_func((char *) CMD_AUDIO_RT_CHANNEL, (char *) 0, &ScriptParsing::sc_audio_rt_channel);
	assign_member_func((char *) CMD_WAVE_SAMPLE_RATE, (char *) 0, &ScriptParsing::sc_wave_file_sample_rate);
	assign_member_func((char *) CMD_HRDWR_PTT,        (char *) 0, &ScriptParsing::sc_hrdwr_ptt);
	assign_member_func((char *) CMD_RIGCAT,           (char *) 0, &ScriptParsing::sc_rigcat);
	assign_member_func((char *) CMD_HAMLIB,           (char *) 0, &ScriptParsing::sc_hamlib);
	assign_member_func((char *) CMD_RC_XMLRPC,        (char *) 0, &ScriptParsing::sc_rc_xmlrpc);
	assign_member_func((char *) CMD_EXEC_MACRO,       (char *) 0, &ScriptParsing::sc_exec_macro);

	assign_member_func((char *) CMD_OPERATOR, (char *) CMD_CALLSIGN, &ScriptParsing::sc_callsign_info);
	assign_member_func((char *) CMD_OPERATOR, (char *) CMD_NAME,     &ScriptParsing::sc_name_info);
	assign_member_func((char *) CMD_OPERATOR, (char *) CMD_QTH,      &ScriptParsing::sc_qth_info);
	assign_member_func((char *) CMD_OPERATOR, (char *) CMD_LOCATOR,  &ScriptParsing::sc_locator_info);
	assign_member_func((char *) CMD_OPERATOR, (char *) CMD_ANTENNA,  &ScriptParsing::sc_antenna_info);

	assign_member_func((char *) CMD_AUDIO_DEVICE, (char *) CMD_OSS_AUDIO,          &ScriptParsing::sc_use_oss_audio_device);
	assign_member_func((char *) CMD_AUDIO_DEVICE, (char *) CMD_OSS_AUDIO_DEV_PATH, &ScriptParsing::sc_oss_audio_device_path);
	assign_member_func((char *) CMD_AUDIO_DEVICE, (char *) CMD_PORT_AUDIO,         &ScriptParsing::sc_use_port_audio_device);
	assign_member_func((char *) CMD_AUDIO_DEVICE, (char *) CMD_PORTA_CAP,          &ScriptParsing::sc_capture_path);
	assign_member_func((char *) CMD_AUDIO_DEVICE, (char *) CMD_PORTA_PLAY,         &ScriptParsing::sc_playback_path);
	assign_member_func((char *) CMD_AUDIO_DEVICE, (char *) CMD_PULSEA,             &ScriptParsing::sc_use_pulse_audio_device);
	assign_member_func((char *) CMD_AUDIO_DEVICE, (char *) CMD_PULSEA_SERVER,      &ScriptParsing::sc_pulse_audio_device_path);

	assign_member_func((char *) CMD_AUDIO_SETTINGS, (char *) CMD_CAPTURE_SAMPLE_RATE,  &ScriptParsing::sc_audio_device_sample_rate_capture);
	assign_member_func((char *) CMD_AUDIO_SETTINGS, (char *) CMD_PLAYBACK_SAMPLE_RATE, &ScriptParsing::sc_audio_device_sample_rate_playback);
	assign_member_func((char *) CMD_AUDIO_SETTINGS, (char *) CMD_AUDIO_CONVERTER,      &ScriptParsing::sc_audio_device_converter);
	assign_member_func((char *) CMD_AUDIO_SETTINGS, (char *) CMD_RX_PPM,               &ScriptParsing::sc_rx_ppm);
	assign_member_func((char *) CMD_AUDIO_SETTINGS, (char *) CMD_TX_PPM,               &ScriptParsing::sc_tx_ppm);
	assign_member_func((char *) CMD_AUDIO_SETTINGS, (char *) CMD_TX_OFFSET,            &ScriptParsing::sc_tx_offset);

}

/** **************************************************************
 * \brief Copy environment to the sub ScriptParsing class
 * \param src Source Class pointer to copy from.
 *****************************************************************/
int ScriptParsing::CopyScriptParsingEnv(ScriptParsing *src)
{
	if(!src || (src == this)) return -1;

	callsign_info(src->callsign_info());
	name_info(src->name_info());
	qth_info(src->qth_info());
	locator_info(src->locator_info());
	antenna_info(src->antenna_info());

	use_oss_audio_device(src->use_oss_audio_device());
	oss_audio_device_path(src->oss_audio_device_path());

	use_port_audio_device(src->use_port_audio_device());
	capture_path(src->capture_path());
	playback_path(src->playback_path());

	use_pulse_audio_device(src->use_pulse_audio_device());
	pulse_audio_device_path(src->pulse_audio_device_path());

	audio_device_sample_rate_capture(src->audio_device_sample_rate_capture());
	audio_device_sample_rate_playback(src->audio_device_sample_rate_playback());
	audio_device_converter(src->audio_device_converter());

	rx_ppm(src->rx_ppm());
	tx_ppm(src->tx_ppm());
	tx_offset(src->tx_offset());

	mono_audio_output(src->mono_audio_output());
	modem_signal_left_right(src->modem_signal_left_right());
	reverse_left_right(src->reverse_left_right());

	ptt_tone_right_channel(src->ptt_tone_right_channel());
	cw_qsk_right_channel(src->cw_qsk_right_channel());
	pseudo_fsk_right_channel(src->pseudo_fsk_right_channel());

	wave_file_sample_rate(src->wave_file_sample_rate());

	hrdw_ptt_right_audio_channel(src->hrdw_ptt_right_audio_channel());
	hrdw_ptt_sep_serial_port(src->hrdw_ptt_sep_serial_port());
	hrdw_ptt_sep_serial_port_path(src->hrdw_ptt_sep_serial_port_path());
	hrdw_ptt_sep_serial_port_rts(src->hrdw_ptt_sep_serial_port_rts());
	hrdw_ptt_sep_serial_port_dtr(src->hrdw_ptt_sep_serial_port_dtr());
	hrdw_ptt_sep_serial_port_rts_v(src->hrdw_ptt_sep_serial_port_rts_v());
	hrdw_ptt_sep_serial_port_dtr_v(src->hrdw_ptt_sep_serial_port_dtr_v());
	hrdw_ptt_start_delay(src->hrdw_ptt_start_delay());
	hrdw_ptt_end_delay(src->hrdw_ptt_end_delay());
	hrdw_ptt_initialize(src->hrdw_ptt_initialize());

	use_rigcat(src->use_rigcat());
	rigcat_desc_file(src->rigcat_desc_file());
	rigcat_device_path(src->rigcat_device_path());
	rigcat_retries(src->rigcat_retries());
	rigcat_retry_interval(src->rigcat_retry_interval());
	rigcat_write_delay(src->rigcat_write_delay());
	rigcat_init_delay(src->rigcat_init_delay());
	rigcat_baud_rate(src->rigcat_baud_rate());
	rigcat_stop_bits(src->rigcat_stop_bits());
	rigcat_commands_echoed(src->rigcat_commands_echoed());
	rigcat_toggle_rts_ptt(src->rigcat_toggle_rts_ptt());
	rigcat_restore_on_close(src->rigcat_restore_on_close());
	rigcat_cat_command_ptt(src->rigcat_cat_command_ptt());
	rigcat_dtr_12v(src->rigcat_dtr_12v());
	rigcat_vsp_enable(src->rigcat_vsp_enable());
	rigcat_initialize(src->rigcat_initialize());


	use_hamlib(src->use_hamlib());
	hamlib_rig(src->hamlib_rig());
	hamlib_retries(src->hamlib_retries());
	hamlib_retry_interval(src->hamlib_retry_interval());
	hamlib_write_delay(src->hamlib_write_delay());
	hamlib_post_write_delay(src->hamlib_post_write_delay());
	hamlib_device_path(src->hamlib_device_path());
	hamlib_baud_rate(src->hamlib_baud_rate());
	hamlib_stop_bits(src->hamlib_stop_bits());
	hamlib_sideband(src->hamlib_sideband());
	hamlib_ptt_hl_command(src->hamlib_ptt_hl_command());
	hamlib_dtr_12(src->hamlib_dtr_12());
	hamlib_rts_12(src->hamlib_rts_12());
	hamlib_rts_cts_flow(src->hamlib_rts_cts_flow());
	hamlib_xon_xoff_flow(src->hamlib_xon_xoff_flow());
	hamlib_advanced_config(src->hamlib_advanced_config());
	hamlib_initialize(src->hamlib_initialize());

	use_xml_rpc(src->use_xml_rpc());
	xml_rpc_mode_bw_delay(src->xml_rpc_mode_bw_delay());
	xml_rpc_initialize(src->xml_rpc_initialize());

	parent(src);
	script_commands(src->script_commands());
	script_command_count(src->script_command_count());

	initialize_function_members();

	return 0;
}

/** **************************************************************
 * \brief Constructor: Copy and initialize function arrays.<br>
 *****************************************************************/
ScriptParsing::ScriptParsing()
{
	size_t count = 0;
	size_t index = 0;
	size_t table_count = 0;

	SCRIPT_COMMANDS * dst_table = 0;
	SCRIPT_COMMANDS * src_table = 0;

	clear_script_parameters(true);

	memset(line_buffer,      0, sizeof(line_buffer));
	memset(error_cmd_buffer, 0, sizeof(error_cmd_buffer));
	memset(error_string,     0, sizeof(error_string));
	memset(error_buffer,     0, sizeof(error_buffer));

	top_cache = 0;
	recursive_count = 0;

	count = sizeof(default_top_level_command_table)/sizeof(SCRIPT_COMMANDS);

	_script_commands  = new SCRIPT_COMMANDS[count];

	if(_script_commands) {
		_script_command_count = count;

		memcpy(_script_commands, default_top_level_command_table, sizeof(SCRIPT_COMMANDS) * _script_command_count);
		
		for(index = 0; index < count; index++) {
			
			src_table   = _script_commands[index].sub_commands;
			table_count = _script_commands[index].sub_command_count;

			if(src_table && table_count) {
				dst_table = new SCRIPT_COMMANDS[table_count];
				
				if(dst_table) {
					memcpy(dst_table, src_table, sizeof(SCRIPT_COMMANDS) * table_count);
					
					_script_commands[index].sub_commands      = dst_table;
					_script_commands[index].sub_command_count = table_count;
				}
			}
		}
	}
	
	
	initialize_function_members();
	
}

/** **************************************************************
 * \brief Destructor
 *****************************************************************/
ScriptParsing::~ScriptParsing()
{
	if(_script_commands) {
		for(int index = 0; index < _script_command_count; index++) {
			if(_script_commands[index].sub_commands)
				delete [] _script_commands[index].sub_commands;
		}
		delete [] _script_commands;
	}
}

/** **************************************************************
 * \brief Dummy callback function for initialization of
 * function pointers.
 * \param sp The calling ScriptParsing Class
 * \param sc Command data structure pointer to the matching script
 * command.
 * \return 0 = No error<br> \< 0 = Error<br>
 *****************************************************************/
int callback_dummy(ScriptParsing *sp, struct script_cmds *sc)
{
	return 0;
}

/** ********************************************************
 * \brief Determine the length of the string with a count
 * limitation.
 * \return signed integer. The number of characters in the
 * array not to excede count limit.
 ***********************************************************/
int ScriptParsing::str_cnt(char * str, int count_limit)
{
	if(!str || (count_limit < 1))
		return 0;
	
	int value = 0;
	
	for(int index = 0; index < count_limit; index++) {
		if(str[index] == 0) break;
		value++;
	}
	
	return value;
}

/** ********************************************************
 * \brief Trim leading and trailing spaces from string.
 * \param s String to modify
 * \return s modified string.
 ***********************************************************/
std::string &ScriptParsing::trim(std::string &s) {
	char *buffer = (char *)0;
	char *dst    = (char *)0;
	char *end    = (char *)0;
	char *src    = (char *)0;
	long count   = s.size();
	
	buffer = new char[count + 1];
	if(!buffer) return s;
	
	memcpy(buffer, s.c_str(), count);
	buffer[count] = 0;
	
	dst = src = buffer;
	end = &buffer[count];
	
	while(src < end) {
		if(*src > ' ') break;
		src++;
	}
	
	if(src > dst) {
		while((dst < end) && (src < end))
			*dst++ = *src++;
		*dst = 0;
	}
	
	while(end >= buffer) {
		if(*end > ' ') break;
		*end-- = 0;
	}
	
	s.assign(buffer);
	
	delete [] buffer;
	
	return s;
}

/** **************************************************************
 * \brief Remove leading/trailing white spaces and quotes.
 * \param buffer Destination buffer
 * \param limit passed buffer size
 * \return void
 *****************************************************************/
void ScriptParsing::trim(char *buffer, size_t limit)
{
	char *s      = (char *)0;
	char *e      = (char *)0;
	char *dst    = (char *)0;
	size_t count = 0;
	
	if(!buffer || limit < 1) {
		return;
	}
	
	for(count = 0; count < limit; count++)
		if(buffer[count] == 0) break;
	
	if(count < 1) return;
	
	s = buffer;
	e = &buffer[count-1];
	
	for(size_t i = 0; i < count; i++) {
		if((*s <= ' ') || (*s == '"')) s++;
		else break;
	}
	
	while(e > s) {
		if((*e <= ' ') || (*e == '"'))
			*e = 0;
		else
			break;
		e--;
	}
	
	dst = buffer;
	for(; s <= e; s++) {
		*dst++ = *s;
	}
	*dst = 0;
}
