//======================================================================
//	script_parsing.h  (FLDIGI)
//
//  Author(s):
//
//	Robert Stiles, KK5VD, Copyright (C) 2014
//
// This is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// This software is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
// =====================================================================

#ifndef __script_parsing__
#define __script_parsing__

#include <string>
#include <stdio.h>

#ifdef __WIN32__
#define PATH_SEPERATOR "\\"
#define PATH_CHAR_SEPERATOR '\\'
#include <direct.h>
#define get_current_dir _getcwd
#else
#define PATH_SEPERATOR "/"
#define PATH_CHAR_SEPERATOR '/'
#include <unistd.h>
#define get_current_dir getcwd
#endif

#define MAX_CMD_PARAMETERS 5
#define MAX_COMMAND_LENGTH 128
#define MAX_PARAMETER_LENGTH FILENAME_MAX
#define MAX_READLINE_LENGTH (FILENAME_MAX+FILENAME_MAX)
#define MAX_SUB_SCRIPTS 5

#define SCRIPT_FILE_TAG ((char *)"FLDIGI_CONFIG")
#define CALL_MEMBER_FN(object,ptrToMember)  ((object).*(ptrToMember))

#define RESET_ALL     0x01
#define RESET_PARTIAL 0X02

#ifdef __LOCALIZED_SCRIPT

#define CMD_OPERATOR                _("OPERATOR")
#define CMD_CALLSIGN                   _("CALLSIGN")
#define CMD_NAME                       _("NAME")
#define CMD_QTH                        _("QTH")
#define CMD_LOCATOR                    _("LOC")
#define CMD_ANTENNA                    _("ANT")
#define CMD_OPERATOR_END            _("END")

#define CMD_AUDIO_DEVICE            _("AUDIO DEVICE")
#define CMD_OSS_AUDIO                  _("OSS")
#define CMD_OSS_AUDIO_DEV_PATH         _("OSS DEV")
#define CMD_PORT_AUDIO                 _("PA")
#define CMD_PORTA_CAP                  _("PA CAPTURE")
#define CMD_PORTA_PLAY                 _("PA PLAYBACK")
#define CMD_PULSEA                     _("PUA")
#define CMD_PULSEA_SERVER              _("PUA SERVER")
#define CMD_AUDIO_DEVICE_END        _("END")

#define CMD_AUDIO_SETTINGS          _("AUDIO SETTINGS")
#define CMD_CAPTURE_SAMPLE_RATE        _("CAPTURE")
#define CMD_PLAYBACK_SAMPLE_RATE       _("PLAYBACK")
#define CMD_AUDIO_CONVERTER            _("CONVERTER")
#define CMD_RX_PPM                     _("RX PPM")
#define CMD_TX_PPM                     _("TX PPM")
#define CMD_TX_OFFSET                  _("TX OFFSET")
#define CMD_AUDIO_SETTINGS_END      _("END")

#define CMD_AUDIO_RT_CHANNEL        _("AUDIO RT CHANNEL")
#define CMD_MONO_AUDIO                 _("MONO AUDIO")
#define CMD_AUDIO_L_R                  _("MODEM LR")
#define CMD_AUDIO_REV_L_R              _("REV LR")
#define CMD_PTT_RIGHT_CHAN             _("PTT RT CHAN")
#define CMD_CW_QSK_RT_CHAN             _("QSK RT CHAN")
#define CMD_PSEUDO_FSK_RT_CHAN         _("FSK RT CHAN")
#define CMD_AUDIO_RT_CHANNEL_END    _("END")

#define CMD_WAVE_SAMPLE_RATE        _("AUDIO WAVE SR")

#define CMD_HRDWR_PTT               _("RIG HRDWR PTT")
#define CMD_HPPT_PTT_RT                _("PTT RT CHAN")
#define CMD_HPTT_SP2                   _("SERIAL PORT")
#define CMD_HPTT_SP2_PATH              _("DEVICE")
#define CMD_HPTT_SP2_RTS               _("RTS")
#define CMD_HPTT_SP2_DTR               _("DTR")
#define CMD_HPTT_SP2_RTS_V             _("RTSV")
#define CMD_HPTT_SP2_DTR_V             _("DTRV")
#define CMD_HPTT_SP2_START_DELAY       _("START PTT DELAY")
#define CMD_HPTT_SP2_END_DELAY         _("END PTT DELAY")
#define CMD_HPTT_SP2_INITIALIZE        _("INIT")
#define CMD_HPTT_UHROUTER              _("UHROUTER")
#define CMD_HRDWR_PTT_END           _("END")

#define CMD_RIGCAT                  _("RIGCAT")
#define CMD_RIGCAT_STATE                _("STATE")
#define CMD_RIGCAT_DESC_FILE            _("DESC FILE")
#define CMD_RIGCAT_DEV_PATH             _("DEV PATH")
#define CMD_RIGCAT_RETRIES              _("RETRIES")
#define CMD_RIGCAT_RETRY_INTERVAL       _("RETRY INT")
#define CMD_RIGCAT_WRITE_DELAY          _("WDELAY")
#define CMD_RIGCAT_INTIAL_DELAY         _("IDELAY")
#define CMD_RIGCAT_BAUD_RATE            _("BRATE")
#define CMD_RIGCAT_STOP_BITS            _("SBITS")
#define CMD_RIGCAT_ECHO                 _("ECHO")
#define CMD_RIGCAT_TOGGLE_RTS_PTT       _("TOGGLE RTS PTT")
#define CMD_RIGCAT_RESTORE              _("RESTORE")
#define CMD_RIGCAT_PTT_COMMAND          _("PTT COMMAND")
#define CMD_RIGCAT_DTR_12V              _("DTR 12V")
#define CMD_RIGCAT_VSP                  _("VSP")
#define CMD_RIGCAT_INITIALIZE           _("INIT")
#define CMD_RIGCAT_END              _("END")

#define CMD_HAMLIB                  _("HAMLIB")
#define CMD_HAMLIB_STATE               _("STATE")
#define CMD_HAMLIB_RIG                 _("RIG")
#define CMD_HAMLIB_DEV_PATH            _("DEV PATH")
#define CMD_HAMLIB_RETRIES             _("RETRIES")
#define CMD_HAMLIB_RETRY_INTERVAL      _("RETRY INT")
#define CMD_HAMLIB_WRITE_DELAY         _("WDELAY")
#define CMD_HAMLIB_POST_WRITE_DELAY    _("PWDELAY")
#define CMD_HAMLIB_BAUD_RATE           _("BRATE")
#define CMD_HAMLIB_STOP_BITS           _("SBITS")
#define CMD_HAMLIB_SIDE_BAND           _("SBAND")
#define CMD_HAMLIB_PTT_COMMAND         _("PTT COMMAND")
#define CMD_HAMLIB_DTR_12V             _("DTR 12V")
#define CMD_HAMLIB_RTS_12V             _("RTS 12V")
#define CMD_HAMLIB_HRDWR_FLOW          _("HRDWR FC")
#define CMD_HAMLIB_SFTWR_FLOW          _("SFTWR FC")
#define CMD_HAMLIB_ADV_CONFIG          _("ADV CONF")
#define CMD_HAMLIB_INITIALIZE          _("INIT")
#define CMD_HAMLIB_END              _("END")

#define CMD_RC_XMLRPC               _("XMLRPC RC")
#define CMD_RC_XMLRPC_STATE            _("STATE")
#define CMD_XMLRPC_BW_DELAY            _("BWDELAY")
#define CMD_XMLRPC_INITIALIZE          _("INIT")
#define CMD_RC_XMLRPC_END           _("END")

#define CMD_EXEC_MACRO              _("EXEC MACRO")
#define CMD_EXEC_MACRO_END          _("END")

#else // __LOCALIZED_SCRIPT

#define CMD_OPERATOR                "OPERATOR"
#define CMD_CALLSIGN                   "CALLSIGN"
#define CMD_NAME                       "NAME"
#define CMD_QTH                        "QTH"
#define CMD_LOCATOR                    "LOC"
#define CMD_ANTENNA                    "ANT"
#define CMD_OPERATOR_END            "END"

#define CMD_AUDIO_DEVICE            "AUDIO DEVICE"
#define CMD_OSS_AUDIO                  "OSS"
#define CMD_OSS_AUDIO_DEV_PATH         "OSS DEV"
#define CMD_PORT_AUDIO                 "PA"
#define CMD_PORTA_CAP                  "PA CAPTURE"
#define CMD_PORTA_PLAY                 "PA PLAYBACK"
#define CMD_PULSEA                     "PUA"
#define CMD_PULSEA_SERVER              "PUA SERVER"
#define CMD_AUDIO_DEVICE_END        "END"

#define CMD_AUDIO_SETTINGS          "AUDIO SETTINGS"
#define CMD_CAPTURE_SAMPLE_RATE        "CAPTURE"
#define CMD_PLAYBACK_SAMPLE_RATE       "PLAYBACK"
#define CMD_AUDIO_CONVERTER            "CONVERTER"
#define CMD_RX_PPM                     "RX PPM"
#define CMD_TX_PPM                     "TX PPM"
#define CMD_TX_OFFSET                  "TX OFFSET"
#define CMD_AUDIO_SETTINGS_END      "END"

#define CMD_AUDIO_RT_CHANNEL        "AUDIO RT CHANNEL"
#define CMD_MONO_AUDIO                 "MONO AUDIO"
#define CMD_AUDIO_L_R                  "MODEM LR"
#define CMD_AUDIO_REV_L_R              "REV LR"
#define CMD_PTT_RIGHT_CHAN             "PTT RT CHAN"
#define CMD_CW_QSK_RT_CHAN             "QSK RT CHAN"
#define CMD_PSEUDO_FSK_RT_CHAN         "FSK RT CHAN"
#define CMD_AUDIO_RT_CHANNEL_END    "END"

#define CMD_WAVE_SAMPLE_RATE        "AUDIO WAVE SR"

#define CMD_HRDWR_PTT               "RIG HRDWR PTT"
#define CMD_HPPT_PTT_RT                "PTT RT CHAN"
#define CMD_HPTT_SP2                   "SERIAL PORT"
#define CMD_HPTT_SP2_PATH              "DEVICE"
#define CMD_HPTT_SP2_RTS               "RTS"
#define CMD_HPTT_SP2_DTR               "DTR"
#define CMD_HPTT_SP2_RTS_V             "RTSV"
#define CMD_HPTT_SP2_DTR_V             "DTRV"
#define CMD_HPTT_SP2_START_DELAY       "START PTT DELAY"
#define CMD_HPTT_SP2_END_DELAY         "END PTT DELAY"
#define CMD_HPTT_SP2_INITIALIZE        "INIT"
#define CMD_HPTT_UHROUTER              "UHROUTER"
#define CMD_HPTT_PARALLEL              "PARALLEL"
#define CMD_HRDWR_PTT_END           "END"

#define CMD_RIGCAT                  "RIGCAT"
#define CMD_RIGCAT_STATE                "STATE"
#define CMD_RIGCAT_DESC_FILE            "DESC FILE"
#define CMD_RIGCAT_DEV_PATH             "DEV PATH"
#define CMD_RIGCAT_RETRIES              "RETRIES"
#define CMD_RIGCAT_RETRY_INTERVAL       "RETRY INT"
#define CMD_RIGCAT_WRITE_DELAY          "WDELAY"
#define CMD_RIGCAT_INTIAL_DELAY         "IDELAY"
#define CMD_RIGCAT_BAUD_RATE            "BRATE"
#define CMD_RIGCAT_STOP_BITS            "SBITS"
#define CMD_RIGCAT_ECHO                 "ECHO"
#define CMD_RIGCAT_TOGGLE_RTS_PTT       "TOGGLE RTS PTT"
#define CMD_RIGCAT_TOGGLE_DTR_PTT       "TOGGLE DTR PTT"
#define CMD_RIGCAT_RESTORE              "RESTORE"
#define CMD_RIGCAT_PTT_COMMAND          "PTT COMMAND"
#define CMD_RIGCAT_RTS_12V              "RTS 12V"
#define CMD_RIGCAT_DTR_12V              "DTR 12V"
#define CMD_RIGCAT_HRDWR_FLOW           "HRDWR FC"
#define CMD_RIGCAT_VSP                  "VSP"
#define CMD_RIGCAT_INITIALIZE           "INIT"
#define CMD_RIGCAT_END              "END"

#define CMD_HAMLIB                  "HAMLIB"
#define CMD_HAMLIB_STATE               "STATE"
#define CMD_HAMLIB_RIG                 "RIG"
#define CMD_HAMLIB_DEV_PATH            "DEV PATH"
#define CMD_HAMLIB_RETRIES             "RETRIES"
#define CMD_HAMLIB_RETRY_INTERVAL      "RETRY INT"
#define CMD_HAMLIB_WRITE_DELAY         "WDELAY"
#define CMD_HAMLIB_POST_WRITE_DELAY    "PWDELAY"
#define CMD_HAMLIB_BAUD_RATE           "BRATE"
#define CMD_HAMLIB_STOP_BITS           "SBITS"
#define CMD_HAMLIB_SIDE_BAND           "SBAND"
#define CMD_HAMLIB_PTT_COMMAND         "PTT COMMAND"
#define CMD_HAMLIB_DTR_12V             "DTR 12V"
#define CMD_HAMLIB_RTS_12V             "RTS 12V"
#define CMD_HAMLIB_HRDWR_FLOW          "HRDWR FC"
#define CMD_HAMLIB_SFTWR_FLOW          "SFTWR FC"
#define CMD_HAMLIB_ADV_CONFIG          "ADV CONF"
#define CMD_HAMLIB_INITIALIZE          "INIT"
#define CMD_HAMLIB_END              "END"

#define CMD_RC_XMLRPC               "XMLRPC RC"
#define CMD_RC_XMLRPC_STATE            "STATE"
#define CMD_XMLRPC_BW_DELAY            "BWDELAY"
#define CMD_XMLRPC_INITIALIZE          "INIT"
#define CMD_RC_XMLRPC_END           "END"

#define CMD_EXEC_MACRO              "EXEC MACRO"
#define CMD_EXEC_MACRO_END          "END"

#endif // __LOCALIZED_SCRIPT


#define SCRIPT_COMMAND 0x0001

#define RECURSIVE_LIMIT 3

//! @enum script_codes
//! Error codes.

//! @typedef SCRIPT_CODES
//! @see script_codes

typedef enum script_codes {
	script_recursive_limit_reached = -100, //!< Reached command recursive limit.
	script_command_not_found = -100,       //!< Script command not found.
	script_errors_reported,                //!< Script Errors found during parsing of file.
	script_file_not_found,                 //!< Script file not found.
	script_path_not_found,                 //!< Script directory path not found.
	script_device_path_not_found,          //!< Script device path not found.
	script_non_script_file,                //!< Opened file not a Script file.
	script_max_sub_script_reached,         //!< Maximum open subscripts reached.
	script_subscript_exec_fail,            //!< Subscript execution fail (internal).
	script_incorrectly_assigned_value,     //!< Incorrect parameter type.
	script_invalid_parameter,              //!< Parameter is not valid.
	script_parameter_error,                //!< Script parameter invalid.
	script_function_parameter_error,       //!< Function parameter error (internal, non script error).
	script_mismatched_quotes,              //!< Opening or closing quotes missing prior to reaching end if line.
	script_command_seperator_missing,      //!< Command missing ':'
	script_args_eol,                       //!< Reached end of args list sooner then expected
	script_param_check_eol,                //!< Reached end of parameter check list sooner then expected
	script_paramter_exceeds_length,        //!< Data length exceeds expectations
	script_memory_allocation_error,        //!< Memory Allocation Error (Non-Script internal Error).
	script_general_error = -1,             //!< General purpose error (undefined erros).
	script_no_errors,                      //!< No Errors
	script_char_match_not_found,           //!< Search char not found (Warning)
	script_end_of_line_reached,            //!< End of line reached (Warning)
	script_end_of_list_reached,            //!< End of list reached (Info)
	script_list,                           //!< List command (Info)
	script_command_handled,                //!< Command was procedded and handled (Info)
} SCRIPT_CODES;


//! @enum paramter_types
//! Parameter type flags. Used to validate the informarion.

//! @typedef PARAM_TYPES
//! @see paramter_types

typedef enum paramter_types {
	p_null = 0,
	p_void,
	p_bool,
	p_char,
	p_int,
	p_unsigned_int,
	p_long,
	p_unsigned_long,
	p_float,
	p_double,
	p_string,
	p_path,
	p_filename,
	p_dev_path,
	p_dev_name,
	p_list,
	p_list_end
} PARAM_TYPES;

class ScriptParsing;

//! @struct script_cmds
//! Vector table of script command string and executing function member.

//! @typedef SCRIPT_COMMANDS
//! @see script_cmds
typedef struct script_cmds {
	char command[MAX_COMMAND_LENGTH];                      //!< Command matching string.
	int  flags;                                            //!< General purpose flags
	size_t command_length;                                 //!< Number of chacters in the command string.
	int  argc;                                             //!< Number of required prarmeters for the command
	char *args[MAX_CMD_PARAMETERS+1];                      //!< String vector table of parameters
	enum paramter_types param_check[MAX_CMD_PARAMETERS+1]; //!< Flags to determine type of parameter.
	const char **valid_values;                             //!< A list of valid paramters.
	int valid_value_count;                                 //!< The number of valid paramters.
	SCRIPT_CODES (ScriptParsing::*func)(struct script_cmds *); //!< The function (member) to execute on positive match
	int (*cb)(ScriptParsing *sp, struct script_cmds *sd);      //!< Call back function for script command
	struct script_cmds * sub_commands;                     //!< List of sub commands
	size_t sub_command_count;                              //!< Number of table entires in the sub table.
} SCRIPT_COMMANDS;

//! @class script_parsing_class
class ScriptParsing {

public:

private:
	pthread_mutex_t ac;            //<! Thread safe data access
	int recursive_count;
	// Storage for each parameter

	std::string _callsign_info;
	std::string _name_info;
	std::string _qth_info;
	std::string _locator_info;
	std::string _antenna_info;

	bool _use_oss_audio_device;
	std::string _oss_audio_device_path;

	bool _use_port_audio_device;
	std::string _capture_path;
	std::string _playback_path;

	bool _use_pulse_audio_device;
	std::string _pulse_audio_device_path;

	std::string _audio_device_sample_rate_capture;
	std::string _audio_device_sample_rate_playback;
	std::string _audio_device_converter;

	int _rx_ppm;
	int _tx_ppm;
	int _tx_offset;

	bool _mono_audio_output;
	bool _modem_signal_left_right;
	bool _reverse_left_right;

	bool _ptt_tone_right_channel;
	bool _cw_qsk_right_channel;
	bool _pseudo_fsk_right_channel;

	int _wave_file_sample_rate;

	bool        _hrdw_ptt_right_audio_channel;
	bool        _hrdw_ptt_sep_serial_port;
	std::string _hrdw_ptt_sep_serial_port_path;
	bool        _hrdw_ptt_sep_serial_port_rts;
	bool        _hrdw_ptt_sep_serial_port_dtr;
	bool        _hrdw_ptt_sep_serial_port_rts_v;
	bool        _hrdw_ptt_sep_serial_port_dtr_v;
	int         _hrdw_ptt_start_delay;
	int         _hrdw_ptt_end_delay;
	bool        _hrdw_ptt_uhrouter;
	bool        _hrdw_ptt_parallel;
	bool        _hrdw_ptt_initialize;

	bool _use_rigcat;
	std::string _rigcat_desc_file;
	std::string _rigcat_device_path;
	int         _rigcat_retries;
	int         _rigcat_retry_interval;
	int         _rigcat_write_delay;
	int         _rigcat_init_delay;
	std::string _rigcat_baud_rate;
	std::string _rigcat_stop_bits;
	bool        _rigcat_commands_echoed;
	bool        _rigcat_toggle_rts_ptt;
	bool        _rigcat_restore_on_close;
	bool        _rigcat_cat_command_ptt;
	bool        _rigcat_rts_12v;
	bool        _rigcat_dtr_12v;
	bool        _rigcat_vsp_enable;
	bool        _rigcat_hrdwr_flow;
	bool        _rigcat_initialize;

	bool _use_hamlib;
	std::string _hamlib_rig;
	int         _hamlib_retries;
	int         _hamlib_retry_interval;
	int         _hamlib_write_delay;
	int         _hamlib_post_write_delay;
	std::string _hamlib_device_path;
	std::string _hamlib_baud_rate;
	std::string _hamlib_stop_bits;
	std::string _hamlib_sideband;
	bool        _hamlib_ptt_hl_command;
	bool        _hamlib_dtr_12;
	bool        _hamlib_rts_12;
	bool        _hamlib_rts_cts_flow;
	bool        _hamlib_xon_xoff_flow;
	std::string _hamlib_advanced_config;
	bool        _hamlib_initialize;

	bool _use_xml_rpc;
	std::string _xml_rpc_mode_bw_delay;
	bool        _xml_rpc_initialize;

	std::string _macro_command;

	// Internal Class Variables.

	std::string _path;
	int _file_type;

	SCRIPT_COMMANDS *_script_commands;    //!< Table of commands and vector functions @see script_cmds
	size_t _script_command_count;      	  //!< Number of assigned positions in the table.

	ScriptParsing *_parent;       //!< Calling ScriptParsing pointer. Primarly used for the reset command on the local 'this' pointer.

	char line_buffer[MAX_READLINE_LENGTH + 1]; //!< Line buffer for script read operations.
	char error_cmd_buffer[MAX_COMMAND_LENGTH];
	char error_string[MAX_COMMAND_LENGTH];
	char error_buffer[MAX_COMMAND_LENGTH + MAX_COMMAND_LENGTH + 1];

	SCRIPT_COMMANDS *top_cache;

public:

	// Access methods for the various parsed script data

	std::string callsign_info(void) { return _callsign_info; }
	void callsign_info(std::string value) { _callsign_info.assign(value); }

	std::string name_info(void) { return _name_info; }
	void name_info(std::string value) { _name_info.assign(value); }

	std::string qth_info(void) { return _qth_info; }
	void qth_info(std::string value) { _qth_info.assign(value); }

	std::string locator_info(void) { return _locator_info; }
	void locator_info(std::string value) { _locator_info.assign(value); }

	std::string antenna_info(void) { return _antenna_info; }
	void antenna_info(std::string value) { _antenna_info.assign(value); }

	bool use_oss_audio_device(void) { return _use_oss_audio_device; }
	void use_oss_audio_device(bool value) { _use_oss_audio_device = value; }

	std::string oss_audio_device_path(void) { return _oss_audio_device_path; }
	void oss_audio_device_path(std::string value) { _oss_audio_device_path.assign(value); }

	bool use_port_audio_device(void) { return _use_port_audio_device; }
	void use_port_audio_device(bool value) { _use_port_audio_device = value; }

	std::string capture_path(void) { return _capture_path; }
	void capture_path(std::string value) { _capture_path.assign(value); }

	std::string playback_path(void) { return _playback_path; }
	void playback_path(std::string value) { _playback_path.assign(value); }

	bool use_pulse_audio_device(void) { return _use_pulse_audio_device; }
	void use_pulse_audio_device(bool value) { _use_pulse_audio_device = value; }

	std::string pulse_audio_device_path(void) { return _pulse_audio_device_path; }
	void pulse_audio_device_path(std::string value) { _pulse_audio_device_path.assign(value); }

	std::string audio_device_sample_rate_capture(void) { return _audio_device_sample_rate_capture; }
	void audio_device_sample_rate_capture(std::string value) { _audio_device_sample_rate_capture.assign(value); }

	std::string audio_device_sample_rate_playback(void) { return _audio_device_sample_rate_playback; }
	void audio_device_sample_rate_playback(std::string value) { _audio_device_sample_rate_playback.assign(value); }

	std::string audio_device_converter(void) { return _audio_device_converter; }
	void audio_device_converter(std::string value) { _audio_device_converter.assign(value); }

	int  rx_ppm(void) { return _rx_ppm; }
	void rx_ppm(int value) { _rx_ppm = value; }

	int  tx_ppm(void) { return _tx_ppm; }
	void tx_ppm(int value) { _tx_ppm = value; }

	int  tx_offset(void) { return _tx_offset; }
	void tx_offset(int value) { _tx_offset = value; }

	bool mono_audio_output(void) { return _mono_audio_output; }
	void mono_audio_output(bool value) { _mono_audio_output = value; }

	bool modem_signal_left_right(void) { return _modem_signal_left_right; }
	void modem_signal_left_right(bool value) { _modem_signal_left_right = value; }

	bool reverse_left_right(void) { return _reverse_left_right; }
	void reverse_left_right(bool value) { _reverse_left_right = value; }

	bool ptt_tone_right_channel(void) { return _ptt_tone_right_channel; }
	void ptt_tone_right_channel(bool value) { _ptt_tone_right_channel = value; }

	bool cw_qsk_right_channel(void) { return _cw_qsk_right_channel; }
	void cw_qsk_right_channel(bool value) { _cw_qsk_right_channel = value; }

	bool pseudo_fsk_right_channel(void) { return _pseudo_fsk_right_channel; }
	void pseudo_fsk_right_channel(bool value) { _pseudo_fsk_right_channel = value; }

	int  wave_file_sample_rate(void) { return _wave_file_sample_rate; }
	void wave_file_sample_rate(int value) { _wave_file_sample_rate = value; }

	bool hrdw_ptt_right_audio_channel(void) { return _hrdw_ptt_right_audio_channel; }
	void hrdw_ptt_right_audio_channel(bool value) { _hrdw_ptt_right_audio_channel = value; }

	bool hrdw_ptt_sep_serial_port(void) { return _hrdw_ptt_sep_serial_port; }
	void hrdw_ptt_sep_serial_port(bool value) { _hrdw_ptt_sep_serial_port = value; }

	std::string hrdw_ptt_sep_serial_port_path(void) { return _hrdw_ptt_sep_serial_port_path; }
	void hrdw_ptt_sep_serial_port_path(std::string value) { _hrdw_ptt_sep_serial_port_path.assign(value); }

	bool hrdw_ptt_sep_serial_port_rts(void) { return _hrdw_ptt_sep_serial_port_rts; }
	void hrdw_ptt_sep_serial_port_rts(bool value) { _hrdw_ptt_sep_serial_port_rts = value; }

	bool hrdw_ptt_sep_serial_port_dtr(void) { return _hrdw_ptt_sep_serial_port_dtr; }
	void hrdw_ptt_sep_serial_port_dtr(bool value) { _hrdw_ptt_sep_serial_port_dtr = value; }

	bool hrdw_ptt_sep_serial_port_rts_v(void) { return _hrdw_ptt_sep_serial_port_rts_v; }
	void hrdw_ptt_sep_serial_port_rts_v(bool value) { _hrdw_ptt_sep_serial_port_rts_v = value; }

	bool hrdw_ptt_sep_serial_port_dtr_v(void) { return _hrdw_ptt_sep_serial_port_dtr_v; }
	void hrdw_ptt_sep_serial_port_dtr_v(bool value) { _hrdw_ptt_sep_serial_port_dtr_v = value; }

	int  hrdw_ptt_start_delay(void) { return _hrdw_ptt_start_delay; }
	void hrdw_ptt_start_delay(int value) { _hrdw_ptt_start_delay = value; }

	int  hrdw_ptt_end_delay(void) { return _hrdw_ptt_end_delay; }
	void hrdw_ptt_end_delay(int value) { _hrdw_ptt_end_delay = value; }

	int  hrdw_ptt_uhrouter(void) { return _hrdw_ptt_uhrouter; }
	void hrdw_ptt_uhrouter(int value) { _hrdw_ptt_uhrouter = value; }

	bool hrdw_ptt_initialize(void) { return _hrdw_ptt_initialize; }
	void hrdw_ptt_initialize(bool value) { _hrdw_ptt_initialize = value; }

	bool hrdw_ptt_parallel(void) { return _hrdw_ptt_parallel; }
	void hrdw_ptt_parallel(bool value) { _hrdw_ptt_parallel = value; }

	bool use_rigcat(void) { return _use_rigcat; }
	void use_rigcat(bool value) { _use_rigcat = value; }

	std::string rigcat_desc_file(void) { return _rigcat_desc_file; }
	void rigcat_desc_file(std::string value) { _rigcat_desc_file.assign(value); }

	std::string rigcat_device_path(void) { return _rigcat_device_path; }
	void rigcat_device_path(std::string value) { _rigcat_device_path.assign(value); }

	int  rigcat_retries(void) { return _rigcat_retries; }
	void rigcat_retries(int value) { _rigcat_retries = value; }

	int  rigcat_retry_interval(void) { return _rigcat_retry_interval; }
	void rigcat_retry_interval(int value) { _rigcat_retry_interval = value; }

	int  rigcat_write_delay(void) { return _rigcat_write_delay; }
	void rigcat_write_delay(int value) { _rigcat_write_delay = value; }

	int  rigcat_init_delay(void) { return _rigcat_init_delay; }
	void rigcat_init_delay(int value) { _rigcat_init_delay = value; }

	std::string rigcat_baud_rate(void) { return _rigcat_baud_rate; }
	void rigcat_baud_rate(std::string value) { _rigcat_baud_rate.assign(value); }

	std::string rigcat_stop_bits(void) { return _rigcat_stop_bits; }
	void rigcat_stop_bits(std::string value) { _rigcat_stop_bits.assign(value); }

	bool rigcat_commands_echoed(void) { return _rigcat_commands_echoed; }
	void rigcat_commands_echoed(bool value) { _rigcat_commands_echoed = value; }

	bool rigcat_toggle_rts_ptt(void) { return _rigcat_toggle_rts_ptt; }
	void rigcat_toggle_rts_ptt(bool value) { _rigcat_toggle_rts_ptt = value; }

	bool rigcat_restore_on_close(void) { return _rigcat_restore_on_close; }
	void rigcat_restore_on_close(bool value) { _rigcat_restore_on_close = value; }

	bool rigcat_cat_command_ptt(void) { return _rigcat_cat_command_ptt; }
	void rigcat_cat_command_ptt(bool value) { _rigcat_cat_command_ptt = value; }

	bool rigcat_rts_12v(void) { return _rigcat_rts_12v; }
	void rigcat_rts_12v(bool value) { _rigcat_rts_12v = value; }

	bool rigcat_dtr_12v(void) { return _rigcat_dtr_12v; }
	void rigcat_dtr_12v(bool value) { _rigcat_dtr_12v = value; }

	bool rigcat_hrdwr_flow(void) { return _rigcat_hrdwr_flow; }
	void rigcat_hrdwr_flow(bool value) { _rigcat_hrdwr_flow = value; }

	bool rigcat_vsp_enable(void) { return _rigcat_vsp_enable; }
	void rigcat_vsp_enable(bool value) { _rigcat_vsp_enable = value; }

	bool rigcat_initialize(void) { return _rigcat_initialize; }
	void rigcat_initialize(bool value) { _rigcat_initialize = value; }

	bool use_hamlib(void) { return _use_hamlib; }
	void use_hamlib(bool value) { _use_hamlib = value; }

	std::string hamlib_rig(void) { return _hamlib_rig; }
	void hamlib_rig(std::string value) { _hamlib_rig.assign(value); }

	int  hamlib_retries(void) { return _hamlib_retries; }
	void hamlib_retries(int value) { _hamlib_retries = value; }

	int  hamlib_retry_interval(void) { return _hamlib_retry_interval; }
	void hamlib_retry_interval(int value) { _hamlib_retry_interval = value; }

	int  hamlib_write_delay(void) { return _hamlib_write_delay; }
	void hamlib_write_delay(int value) { _hamlib_write_delay = value; }

	int  hamlib_post_write_delay(void) { return _hamlib_post_write_delay; }
	void hamlib_post_write_delay(int value) { _hamlib_post_write_delay = value; }

	std::string hamlib_device_path(void) { return _hamlib_device_path; }
	void hamlib_device_path(std::string value) { _hamlib_device_path.assign(value); }

	std::string hamlib_baud_rate(void) { return _hamlib_baud_rate; }
	void hamlib_baud_rate(std::string value) { _hamlib_baud_rate.assign(value); }

	std::string hamlib_stop_bits(void) { return _hamlib_stop_bits; }
	void hamlib_stop_bits(std::string value) { _hamlib_stop_bits.assign(value); }

	std::string hamlib_sideband(void) { return _hamlib_sideband; }
	void hamlib_sideband(std::string value) { _hamlib_sideband.assign(value); }

	bool hamlib_ptt_hl_command(void) { return _hamlib_ptt_hl_command; }
	void hamlib_ptt_hl_command(bool value) { _hamlib_ptt_hl_command = value; }

	bool hamlib_dtr_12(void) { return _hamlib_dtr_12; }
	void hamlib_dtr_12(bool value) { _hamlib_dtr_12 = value; }

	bool hamlib_rts_12(void) { return _hamlib_rts_12; }
	void hamlib_rts_12(bool value) { _hamlib_rts_12 = value; }

	bool hamlib_rts_cts_flow(void) { return _hamlib_rts_cts_flow; }
	void hamlib_rts_cts_flow(bool value) { _hamlib_rts_cts_flow = value; }

	bool hamlib_xon_xoff_flow(void) { return _hamlib_xon_xoff_flow; }
	void hamlib_xon_xoff_flow(bool value) { _hamlib_xon_xoff_flow = value; }

	std::string hamlib_advanced_config(void) { return _hamlib_advanced_config; }
	void hamlib_advanced_config(std::string value) { _hamlib_advanced_config.assign(value); }

	bool hamlib_initialize(void) { return _hamlib_initialize; }
	void hamlib_initialize(bool value) { _hamlib_initialize = value; }

	bool use_xml_rpc(void) { return _use_xml_rpc; }
	void use_xml_rpc(bool value) { _use_xml_rpc = value; }

	std::string xml_rpc_mode_bw_delay(void) { return _xml_rpc_mode_bw_delay; }
	void xml_rpc_mode_bw_delay(std::string value) { _xml_rpc_mode_bw_delay.assign(value); }

	bool xml_rpc_initialize(void) { return _xml_rpc_initialize; }
	void xml_rpc_initialize(bool value) { _xml_rpc_initialize = value; }

	std::string macro_command(void) { return _macro_command; }
	void macro_command(std::string value) { _macro_command.assign(value); }

	SCRIPT_COMMANDS * script_commands(void) { return _script_commands;	}
	void script_commands(SCRIPT_COMMANDS *ptr) { if(ptr) _script_commands = ptr; }

	size_t script_command_count(void) { return _script_command_count;	}
	void script_command_count(size_t value) { _script_command_count = value; }


	// Internal script fuctions

	SCRIPT_CODES parse_commands(char *file_name_path); //!< The calling function to parse a script file.
	SCRIPT_CODES read_file(FILE *fd, SCRIPT_COMMANDS *cur_list, size_t limit, int *line_number);

	ScriptParsing *parent(void) { return _parent; }
	void parent(ScriptParsing *value) { _parent = value; }


	int assign_callback(const char *scriptCommand, int (*cb)(ScriptParsing *sp, SCRIPT_COMMANDS *sc));
	int assign_valid_parameters(const char *command, const char **array, const int array_count);

	void defaults(bool all);
	ScriptParsing();
	~ScriptParsing();

private:
	// Internal class routines

	SCRIPT_CODES sc_dummy(struct script_cmds *);
	SCRIPT_CODES remove_crlf_comments(char *data, char *limit, size_t &count);
	SCRIPT_CODES parse_single_command(SCRIPT_COMMANDS **cur_list, size_t *limit, char *data, size_t buffer_size);
	SCRIPT_CODES parse_hierarchy_command(char *data, size_t buffer_size);

	std::string path(void) { return _path; }
	int file_type(void) { return _file_type; }
	void file_type(int value) { _file_type = value; }

	// Parsing/varification routine for each of the commands.

	SCRIPT_CODES check_parameters(struct script_cmds *);
	SCRIPT_CODES check_filename(char *filename, char *full_name_path, size_t limit);
	SCRIPT_CODES check_path(const char *);
	SCRIPT_CODES check_numbers(char *, paramter_types p);
	SCRIPT_CODES check_dev_path(const char *path);
	SCRIPT_CODES check_bool(const char *value, bool &flag);

	// Script handling code.

	inline SCRIPT_CODES test_on_off_state(bool &state, char *string, char *true_state);

	SCRIPT_COMMANDS * search_command(SCRIPT_COMMANDS * table, size_t limit, const char *command);
	SCRIPT_CODES copy_string_uppercase(char *buffer, char *cPtr, char *ePtr, size_t limit);
	SCRIPT_CODES parse_parameters(char *s, char *d, SCRIPT_COMMANDS *matching_command);

	int call_callback(SCRIPT_COMMANDS *cb_data);
	int check_parameters_from_list(SCRIPT_COMMANDS *sc);
	int CopyScriptParsingEnv(ScriptParsing *src);
	void clear_script_parameters(bool all);

	void assign_member_func(char *top_command, char *sub_command, SCRIPT_CODES (ScriptParsing::*func)(struct script_cmds *));

	//void assign_func(size_t pos, calling_func func, size_t limit);
	void to_uppercase(char *str, int limit);
	void to_uppercase(std::string &str);
	void trim(char *buffer, size_t size);
	std::string &trim(std::string &s);
	void initialize_function_members(void);

	int str_cnt(char * str, int count_limit);

	char * script_error_string(SCRIPT_CODES error_no, int line_number, char *cmd);
	char * skip_alpha_numbers(char * data, char *limit, SCRIPT_CODES &error);
	char * skip_characters(char * data, char *limit, SCRIPT_CODES &error);
	char * skip_numbers(char * data, char *limit, SCRIPT_CODES &error);
	char * skip_spaces(char * data, char *limit, SCRIPT_CODES &error);
	char * skip_to_character(char c, char * data, char *limit, SCRIPT_CODES &error);
	char * skip_white_spaces(char * data, char *limit, SCRIPT_CODES &error);

	// Command Processing routines

	// Top level Members

	SCRIPT_CODES sc_operator(struct script_cmds *cmd);
	SCRIPT_CODES sc_audio_device(struct script_cmds *cmd);
	SCRIPT_CODES sc_audio_settings(struct script_cmds *cmd);
	SCRIPT_CODES sc_audio_rt_channel(struct script_cmds *cmd);
	SCRIPT_CODES sc_wave_file_sample_rate(struct script_cmds *cmd);
	SCRIPT_CODES sc_hrdwr_ptt(struct script_cmds *cmd);
	SCRIPT_CODES sc_rigcat(struct script_cmds *cmd);
	SCRIPT_CODES sc_hamlib(struct script_cmds *cmd);
	SCRIPT_CODES sc_rc_xmlrpc(struct script_cmds *cmd);
	SCRIPT_CODES sc_exec_macro(struct script_cmds *cmd);


	// Sub level members

	SCRIPT_CODES sc_callsign_info(struct script_cmds *);
	SCRIPT_CODES sc_name_info(struct script_cmds *);
	SCRIPT_CODES sc_qth_info(struct script_cmds *);
	SCRIPT_CODES sc_locator_info(struct script_cmds *);
	SCRIPT_CODES sc_antenna_info(struct script_cmds *);

	SCRIPT_CODES sc_use_oss_audio_device(struct script_cmds *);
	SCRIPT_CODES sc_oss_audio_device_path(struct script_cmds *);
	SCRIPT_CODES sc_use_port_audio_device(struct script_cmds *);
	SCRIPT_CODES sc_capture_path(struct script_cmds *);
	SCRIPT_CODES sc_playback_path(struct script_cmds *);
	SCRIPT_CODES sc_use_pulse_audio_device(struct script_cmds *);
	SCRIPT_CODES sc_pulse_audio_device_path(struct script_cmds *);

	SCRIPT_CODES sc_audio_device_sample_rate_capture(struct script_cmds *);
	SCRIPT_CODES sc_audio_device_sample_rate_playback(struct script_cmds *);
	SCRIPT_CODES sc_audio_device_converter(struct script_cmds *);
	SCRIPT_CODES sc_rx_ppm(struct script_cmds *);
	SCRIPT_CODES sc_tx_ppm(struct script_cmds *);
	SCRIPT_CODES sc_tx_offset(struct script_cmds *);

	SCRIPT_CODES sc_mono_audio_output(struct script_cmds *);
	SCRIPT_CODES sc_modem_signal_left_right(struct script_cmds *);
	SCRIPT_CODES sc_reverse_left_right(struct script_cmds *);
	SCRIPT_CODES sc_ptt_tone_right_channel(struct script_cmds *);
	SCRIPT_CODES sc_cw_qsk_right_channel(struct script_cmds *);
	SCRIPT_CODES sc_pseudo_fsk_right_channel(struct script_cmds *);
	SCRIPT_CODES sc_hrdw_ptt_right_audio_channel(struct script_cmds *);

	SCRIPT_CODES sc_hrdw_ptt_sep_serial_port(struct script_cmds *);
	SCRIPT_CODES sc_hrdw_ptt_sep_serial_port_path(struct script_cmds *);
	SCRIPT_CODES sc_hrdw_ptt_sep_serial_port_rts(struct script_cmds *);
	SCRIPT_CODES sc_hrdw_ptt_sep_serial_port_dtr(struct script_cmds *);
	SCRIPT_CODES sc_hrdw_ptt_sep_serial_port_rts_v(struct script_cmds *);
	SCRIPT_CODES sc_hrdw_ptt_sep_serial_port_dtr_v(struct script_cmds *);
	SCRIPT_CODES sc_hrdw_ptt_start_delay(struct script_cmds *);
	SCRIPT_CODES sc_hrdw_ptt_end_delay(struct script_cmds *);
	SCRIPT_CODES sc_hrdw_ptt_initialize(struct script_cmds *);

	SCRIPT_CODES sc_use_rigcat(struct script_cmds *);
	SCRIPT_CODES sc_rigcat_desc_file(struct script_cmds *);
	SCRIPT_CODES sc_rigcat_device_path(struct script_cmds *);
	SCRIPT_CODES sc_rigcat_retries(struct script_cmds *);
	SCRIPT_CODES sc_rigcat_retry_interval(struct script_cmds *);
	SCRIPT_CODES sc_rigcat_write_delay(struct script_cmds *);
	SCRIPT_CODES sc_rigcat_init_delay(struct script_cmds *);
	SCRIPT_CODES sc_rigcat_baud_rate(struct script_cmds *);
	SCRIPT_CODES sc_rigcat_stop_bits(struct script_cmds *);
	SCRIPT_CODES sc_rigcat_commands_echoed(struct script_cmds *);
	SCRIPT_CODES sc_rigcat_toggle_rts_ptt(struct script_cmds *);
	SCRIPT_CODES sc_rigcat_restore_on_close(struct script_cmds *);
	SCRIPT_CODES sc_rigcat_cat_command_ptt(struct script_cmds *);
	SCRIPT_CODES sc_rigcat_dtr_12v(struct script_cmds *);
	SCRIPT_CODES sc_rigcat_vsp_enable(struct script_cmds *);
	SCRIPT_CODES sc_rigcat_initialize(struct script_cmds *);

	SCRIPT_CODES sc_use_hamlib(struct script_cmds *);
	SCRIPT_CODES sc_hamlib_rig(struct script_cmds *);
	SCRIPT_CODES sc_hamlib_retries(struct script_cmds *);
	SCRIPT_CODES sc_hamlib_retry_interval(struct script_cmds *);
	SCRIPT_CODES sc_hamlib_write_delay(struct script_cmds *);
	SCRIPT_CODES sc_hamlib_post_write_delay(struct script_cmds *);
	SCRIPT_CODES sc_hamlib_device_path(struct script_cmds *);
	SCRIPT_CODES sc_hamlib_baud_rate(struct script_cmds *);
	SCRIPT_CODES sc_hamlib_stop_bits(struct script_cmds *);
	SCRIPT_CODES sc_hamlib_sideband(struct script_cmds *);
	SCRIPT_CODES sc_hamlib_ptt_hl_command(struct script_cmds *);
	SCRIPT_CODES sc_hamlib_dtr_12(struct script_cmds *);
	SCRIPT_CODES sc_hamlib_rts_12(struct script_cmds *);
	SCRIPT_CODES sc_hamlib_rts_cts_flow(struct script_cmds *);
	SCRIPT_CODES sc_hamlib_xon_xoff_flow(struct script_cmds *);
	SCRIPT_CODES sc_hamlib_advanced_config(struct script_cmds *);
	SCRIPT_CODES sc_hamlib_initialize(struct script_cmds *);

	SCRIPT_CODES sc_use_xml_rpc(struct script_cmds *);
	SCRIPT_CODES sc_xml_rpc_mode_bw_delay(struct script_cmds *);
	SCRIPT_CODES sc_xml_rpc_initialize(struct script_cmds *);
	
};

int callback_dummy(ScriptParsing *sp, struct script_cmds *sc);

#endif /* defined(__script_parsing__) */
