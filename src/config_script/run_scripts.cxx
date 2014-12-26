/** **************************************************************
 \page run_scripts Executing Script Parsing Class

 \par run_script.cxx (FLDIGI)

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
 This code is the interface between the parsing engine and the
 GUI code. The processing of the GUI is performed via 'C' style
 callback fuctions.
 <br>
 \par NOTE:
 Do not call process_xxx() functions directly. Must be called from the
 ScriptParsing Class.
 *******************************************************************/

#include <cstdlib>
#include <cstdarg>
#include <string>
#include <fstream>
#include <algorithm>
#include <map>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <ctime>
#include <vector>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <errno.h>
#include <time.h>
#include <pthread.h>
#include <libgen.h>
#include <ctype.h>
#include <sys/time.h>

#include "config.h"

#include <sys/types.h>

#ifdef __WOE32__
#  ifdef __CYGWIN__
#    include <w32api/windows.h>
#  else
#    include <windows.h>
#  endif
#endif

#include <cstdlib>
#include <cstdarg>
#include <string>
#include <fstream>
#include <algorithm>
#include <map>

#ifndef __WOE32__
#include <sys/wait.h>
#endif

#include "gettext.h"
#include "fl_digi.h"

#include <FL/Fl.H>
#include <FL/fl_ask.H>
#include <FL/Fl_Pixmap.H>
#include <FL/Fl_Image.H>
//#include <FL/Fl_Tile.H>
#include <FL/x.H>
#include <FL/Fl_Help_Dialog.H>
#include <FL/Fl_Progress.H>
#include <FL/Fl_Tooltip.H>
#include <FL/Fl_Tabs.H>
#include <FL/Fl_Multiline_Input.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Pack.H>
#include <FL/filename.H>
#include <FL/fl_ask.H>

#include "waterfall.h"
#include "raster.h"
#include "progress.h"
#include "Panel.h"

#include "main.h"
#include "threads.h"
#include "trx.h"
#if USE_HAMLIB
#include "hamlib.h"
#endif
#include "timeops.h"
#include "rigio.h"
#include "nullmodem.h"
#include "psk.h"
#include "cw.h"
#include "mfsk.h"
#include "wefax.h"
#include "wefax-pic.h"
#include "navtex.h"
#include "mt63.h"
#include "view_rtty.h"
#include "olivia.h"
#include "contestia.h"
#include "thor.h"
#include "dominoex.h"
#include "feld.h"
#include "throb.h"
//#include "pkt.h"
#include "wwv.h"
#include "analysis.h"
#include "fftscan.h"
#include "ssb.h"

#include "smeter.h"
#include "pwrmeter.h"

#include "ascii.h"
#include "globals.h"
#include "misc.h"
#include "FTextRXTX.h"

#include "confdialog.h"
#include "configuration.h"
#include "status.h"

#include "macros.h"
#include "macroedit.h"
#include "logger.h"
#include "lookupcall.h"

#include "font_browser.h"

#include "icons.h"
#include "pixmaps.h"

#include "rigsupport.h"

#include "qrunner.h"

#include "Viewer.h"
#include "soundconf.h"

#include "htmlstrings.h"
#	include "xmlrpc.h"
#if BENCHMARK_MODE
#	include "benchmark.h"
#endif

#include "debug.h"
#include "re.h"
#include "network.h"
#include "spot.h"
#include "dxcc.h"
#include "locator.h"
#include "notify.h"

#include "logbook.h"

#include "rx_extract.h"
#include "speak.h"
#include "flmisc.h"

#include "arq_io.h"
#include "data_io.h"
#include "kmlserver.h"

#include "notifydialog.h"
#include "macroedit.h"
#include "rx_extract.h"
#include "wefax-pic.h"
#include "charsetdistiller.h"
#include "charsetlist.h"
#include "outputencoder.h"
#include "record_loader.h"
#include "record_browse.h"
#include "fileselect.h"

#include "script_parsing.h"
#include "run_script.h"

pthread_mutex_t mutex_script_io = PTHREAD_MUTEX_INITIALIZER;

extern std::string ScriptsDir;


void script_execute(void *);
void script_execute(const char *filename, bool queue_flag);

/** ********************************************************
 * \brief Assign Call Sign.
 * \param sp Access to ScritpParsing members.
 * \param sc Access to SCRIPT_COMMANDS structure variables.
 * \return 0 (no error) Other (error)
 * \par Note:
 * This string storage can be assigned to anything. User
 * should follow the limitations imposed by the rules
 * of the host country.
 ***********************************************************/
int process_callsign_info(ScriptParsing *sp, SCRIPT_COMMANDS *sc)
{
	if(!sp)
		return script_function_parameter_error;

	std::string str_data;
	str_data.assign(sp->callsign_info());

	if(str_data.empty())
		return script_invalid_parameter;

	inpMyCallsign->value(str_data.c_str());
	inpMyCallsign->do_callback();

	return script_no_errors;
}

/** ********************************************************
 * \brief Operator Name
 * \param sp Access to ScritpParsing members.
 * \param sc Access to SCRIPT_COMMANDS structure variables.
 * \return 0 (no error) Other (error)
 ***********************************************************/
int process_name_info(ScriptParsing *sp, SCRIPT_COMMANDS *sc)
{
	if(!sp)
		return script_function_parameter_error;

	std::string str_data;
	str_data.assign(sp->name_info());

	if(str_data.empty())
		return script_invalid_parameter;

	inpMyName->value(str_data.c_str());
	inpMyName->do_callback();

	return script_no_errors;

}

/** ********************************************************
 * \brief QTH Location of Operator
 * \param sp Access to ScritpParsing members.
 * \param sc Access to SCRIPT_COMMANDS structure variables.
 * \return 0 (no error) Other (error)
 ***********************************************************/
int process_qth_info(ScriptParsing *sp, SCRIPT_COMMANDS *sc)
{
	if(!sp)
		return script_function_parameter_error;

	std::string str_data;
	str_data.assign(sp->qth_info());

	if(str_data.empty())
		return script_invalid_parameter;

	inpMyQth->value(str_data.c_str());
	inpMyQth->do_callback();

	return script_no_errors;
}

/** ********************************************************
 * \brief Assign Locator
 * \param sp Access to ScritpParsing members.
 * \param sc Access to SCRIPT_COMMANDS structure variables.
 * \return 0 (no error) Other (error)
 ***********************************************************/
int process_locator_info(ScriptParsing *sp, SCRIPT_COMMANDS *sc)
{
	if(!sp)
		return script_function_parameter_error;

	std::string str_data;
	str_data.assign(sp->locator_info());

	if(str_data.empty())
		return script_invalid_parameter;

	inpMyLocator->value(str_data.c_str());
	inpMyLocator->do_callback();

	return script_no_errors;
}

/** ********************************************************
 * \brief Assign Antenna information
 * \param sp Access to ScritpParsing members.
 * \param sc Access to SCRIPT_COMMANDS structure variables.
 * \return 0 (no error) Other (error)
 ***********************************************************/
int process_antenna_info(ScriptParsing *sp, SCRIPT_COMMANDS *sc)
{
	if(!sp)
		return script_function_parameter_error;

	std::string str_data;
	str_data.assign(sp->antenna_info());

	if(str_data.empty())
		return script_invalid_parameter;

	inpMyAntenna->value(str_data.c_str());
	inpMyAntenna->do_callback();

	return script_no_errors;
}

/** ********************************************************
 * \brief
 * \param sp Access to ScritpParsing members.
 * \param sc Access to SCRIPT_COMMANDS structure variables.
 * \return 0 (no error) Other (error)
 ***********************************************************/
int process_use_oss_audio_device(ScriptParsing *sp, SCRIPT_COMMANDS *sc)
{
#if USE_OSS
	if(!sp)
		return script_function_parameter_error;

	bool value = sp->use_oss_audio_device();

	btnAudioIO[0]->value(value);
	btnAudioIO[0]->do_callback();

#endif // USE_OSS
	return script_no_errors;
}

/** ********************************************************
 * \brief
 * \param sp Access to ScritpParsing members.
 * \param sc Access to SCRIPT_COMMANDS structure variables.
 * \return 0 (no error) Other (error)
 ***********************************************************/
int process_oss_audio_device_path(ScriptParsing *sp, SCRIPT_COMMANDS *sc)
{
#if USE_OSS
	if(!sp)
		return script_function_parameter_error;

	std::string str_data;
	std::string valid_data;

	str_data.assign(sp->oss_audio_device_path());
	if(str_data.empty())
		return script_invalid_parameter;

	int index = 0;
	bool found = false;
	int count = menuOSSDev->menubutton()->size();

	for (index = 0; index < count; index++ ) {
		const Fl_Menu_Item &item = menuOSSDev->menubutton()->menu()[index];
		valid_data.assign(item.label());
		if(!valid_data.empty()) {
			if(strncmp(valid_data.c_str(), str_data.c_str(), FL_PATH_MAX) == 0) {
				found = true;
				break;
			}
		}
	}

	if(!found)
		return script_invalid_parameter;

	menuOSSDev->value(index);
	menuOSSDev->do_callback();

#endif // USE_OSS
	return script_no_errors;
}

/** ********************************************************
 * \brief
 * \param sp Access to ScritpParsing members.
 * \param sc Access to SCRIPT_COMMANDS structure variables.
 * \return 0 (no error) Other (error)
 ***********************************************************/
int process_use_port_audio_device(ScriptParsing *sp, SCRIPT_COMMANDS *sc)
{
#if USE_PORTAUDIO
	if(!sp)
		return script_function_parameter_error;

	bool value = sp->use_port_audio_device();

	btnAudioIO[1]->value(value);
	btnAudioIO[1]->do_callback();

#endif // USE_PORTAUDIO
	return script_no_errors;
}

/** ********************************************************
 * \brief
 * \param sp Access to ScritpParsing members.
 * \param sc Access to SCRIPT_COMMANDS structure variables.
 * \return 0 (no error) Other (error)
 ***********************************************************/
int process_capture_path(ScriptParsing *sp, SCRIPT_COMMANDS *sc)
{
#if USE_PORTAUDIO
	if(!sp)
		return script_function_parameter_error;

	std::string str_data;

	str_data.assign(sp->capture_path());
	if(str_data.empty())
		return script_invalid_parameter;

	int index = menuPortInDev->find_index(str_data.c_str());
	if(!index)
		return script_invalid_parameter;

	menuPortInDev->value(index);
	menuPortInDev->do_callback();
	
#endif // USE_PORTAUDIO
	return script_no_errors;
}

/** ********************************************************
 * \brief
 * \param sp Access to ScritpParsing members.
 * \param sc Access to SCRIPT_COMMANDS structure variables.
 * \return 0 (no error) Other (error)
 ***********************************************************/
int process_playback_path(ScriptParsing *sp, SCRIPT_COMMANDS *sc)
{
#if USE_PORTAUDIO
	if(!sp)
		return script_function_parameter_error;

	std::string str_data;

	str_data.assign(sp->capture_path());
	if(str_data.empty())
		return script_invalid_parameter;

	int index = menuPortOutDev->find_index(str_data.c_str());
	if(!index)
		return script_invalid_parameter;

	menuPortOutDev->value(index);
	menuPortOutDev->do_callback();

#endif // USE_PORTAUDIO
	return script_no_errors;
}

/** ********************************************************
 * \brief
 * \param sp Access to ScritpParsing members.
 * \param sc Access to SCRIPT_COMMANDS structure variables.
 * \return 0 (no error) Other (error)
 ***********************************************************/
int process_use_pulse_audio_device(ScriptParsing *sp, SCRIPT_COMMANDS *sc)
{
#if USE_PULSEAUDIO
	if(!sp)
		return script_function_parameter_error;

	bool value = sp->use_pulse_audio_device();

	btnAudioIO[2]->value(value);
	btnAudioIO[2]->do_callback();

#endif // USE_PULSEAUDIO
	return script_no_errors;
}

/** ********************************************************
 * \brief
 * \param sp Access to ScritpParsing members.
 * \param sc Access to SCRIPT_COMMANDS structure variables.
 * \return 0 (no error) Other (error)
 ***********************************************************/
int process_pulse_audio_device_path(ScriptParsing *sp, SCRIPT_COMMANDS *sc)
{
#if USE_PULSEAUDIO
	if(!sp)
		return script_function_parameter_error;

	std::string str_data;

	str_data.assign(sp->pulse_audio_device_path());

	if(str_data.empty())
		return script_invalid_parameter;

	inpPulseServer->value(str_data.c_str());
	inpPulseServer->do_callback();

#endif // USE_PULSEAUDIO
	return script_no_errors;
}

/** ********************************************************
 * \brief
 * \param sp Access to ScritpParsing members.
 * \param sc Access to SCRIPT_COMMANDS structure variables.
 * \return 0 (no error) Other (error)
 ***********************************************************/
int process_audio_device_sample_rate_capture(ScriptParsing *sp, SCRIPT_COMMANDS *sc)
{
	if(!sp)
		return script_function_parameter_error;

	std::string str_data;

	str_data.assign(sp->audio_device_sample_rate_capture());
	if(str_data.empty())
		return script_invalid_parameter;

	int index = menuInSampleRate->find_index(str_data.c_str());
	if(!index) {
		str_data.append(" (native)");
		index = menuInSampleRate->find_index(str_data.c_str());
	}

	if(!index)
		return script_invalid_parameter;

	menuInSampleRate->index(index);
	menuInSampleRate->do_callback();

	return script_no_errors;
}

/** ********************************************************
 * \brief
 * \param sp Access to ScritpParsing members.
 * \param sc Access to SCRIPT_COMMANDS structure variables.
 * \return 0 (no error) Other (error)
 ***********************************************************/
int process_audio_device_sample_rate_playback(ScriptParsing *sp, SCRIPT_COMMANDS *sc)
{
	if(!sp)
		return script_function_parameter_error;

	std::string str_data;

	str_data.assign(sp->audio_device_sample_rate_playback());
	if(str_data.empty())
		return script_invalid_parameter;

	int index = menuInSampleRate->find_index(str_data.c_str());
	if(!index) {
		str_data.append(" (native)");
		index = menuInSampleRate->find_index(str_data.c_str());
	}

	if(!index)
		return script_invalid_parameter;

	menuOutSampleRate->index(index);
	menuOutSampleRate->do_callback();

	return script_no_errors;

}

/** ********************************************************
 * \brief
 * \param sp Access to ScritpParsing members.
 * \param sc Access to SCRIPT_COMMANDS structure variables.
 * \return 0 (no error) Other (error)
 ***********************************************************/
int process_audio_device_converter(ScriptParsing *sp, SCRIPT_COMMANDS *sc)
{
	if(!sp)
		return script_function_parameter_error;

	std::string str_data;
	std::string append_string;

	str_data.assign(sp->audio_device_converter());
	if(str_data.empty())
		return script_invalid_parameter;

	append_string.assign(" Sinc Interpolator");
	if(str_data.find(append_string) != std::string::npos)
		append_string.clear();

	if(append_string.empty()) {
		append_string.assign(" Interpolator");
		if(str_data.find(append_string) != std::string::npos) {
			append_string.clear();
		}
	}

	str_data.append(append_string);

	int index = menuInSampleRate->find_index(str_data.c_str());
	if(!index)
		return script_invalid_parameter;

	menuInSampleRate->index(index);
	menuInSampleRate->do_callback();

	return script_no_errors;
}

/** ********************************************************
 * \brief
 * \param sp Access to ScritpParsing members.
 * \param sc Access to SCRIPT_COMMANDS structure variables.
 * \return 0 (no error) Other (error)
 ***********************************************************/
int process_rx_ppm(ScriptParsing *sp, SCRIPT_COMMANDS *sc)
{
	if(!sp)
		return script_function_parameter_error;

	int value = sp->rx_ppm();

	cntRxRateCorr->value(value);
	cntRxRateCorr->do_callback();

	return script_no_errors;
}

/** ********************************************************
 * \brief
 * \param sp Access to ScritpParsing members.
 * \param sc Access to SCRIPT_COMMANDS structure variables.
 * \return 0 (no error) Other (error)
 ***********************************************************/
int process_tx_ppm(ScriptParsing *sp, SCRIPT_COMMANDS *sc)
{
	if(!sp)
		return script_function_parameter_error;

	int value = sp->tx_ppm();

	cntTxRateCorr->value(value);
	cntTxRateCorr->do_callback();

	return script_no_errors;
}

/** ********************************************************
 * \brief
 * \param sp Access to ScritpParsing members.
 * \param sc Access to SCRIPT_COMMANDS structure variables.
 * \return 0 (no error) Other (error)
 ***********************************************************/
int process_tx_offset(ScriptParsing *sp, SCRIPT_COMMANDS *sc)
{
	if(!sp)
		return script_function_parameter_error;

	int value = sp->tx_offset();

	cntTxOffset->value(value);
	cntTxOffset->do_callback();

	return script_no_errors;
}

/** ********************************************************
 * \brief
 * \param sp Access to ScritpParsing members.
 * \param sc Access to SCRIPT_COMMANDS structure variables.
 * \return 0 (no error) Other (error)
 ***********************************************************/
int process_mono_audio_output(ScriptParsing *sp, SCRIPT_COMMANDS *sc)
{
	if(!sp)
		return script_function_parameter_error;

	bool value = sp->mono_audio_output();

	chkForceMono->value(value);
	chkForceMono->do_callback();

	return script_no_errors;
}

/** ********************************************************
 * \brief
 * \param sp Access to ScritpParsing members.
 * \param sc Access to SCRIPT_COMMANDS structure variables.
 * \return 0 (no error) Other (error)
 ***********************************************************/
int process_modem_signal_left_right(ScriptParsing *sp, SCRIPT_COMMANDS *sc)
{
	if(!sp)
		return script_function_parameter_error;

	bool value = sp->modem_signal_left_right();

	chkAudioStereoOut->value(value);
	chkAudioStereoOut->do_callback();

	return script_no_errors;
}

/** ********************************************************
 * \brief
 * \param sp Access to ScritpParsing members.
 * \param sc Access to SCRIPT_COMMANDS structure variables.
 * \return 0 (no error) Other (error)
 ***********************************************************/
int process_reverse_left_right(ScriptParsing *sp, SCRIPT_COMMANDS *sc)
{
	if(!sp)
		return script_function_parameter_error;

	bool value = sp->reverse_left_right();

	chkReverseAudio->value(value);
	chkReverseAudio->do_callback();

	return script_no_errors;
}

/** ********************************************************
 * \brief
 * \param sp Access to ScritpParsing members.
 * \param sc Access to SCRIPT_COMMANDS structure variables.
 * \return 0 (no error) Other (error)
 ***********************************************************/
int process_ptt_tone_right_channel(ScriptParsing *sp, SCRIPT_COMMANDS *sc)
{
	if(!sp)
		return script_function_parameter_error;

	bool value = sp->ptt_tone_right_channel();

	btnPTTrightchannel2->value(value);
	btnPTTrightchannel2->do_callback();

	return script_no_errors;
}

/** ********************************************************
 * \brief
 * \param sp Access to ScritpParsing members.
 * \param sc Access to SCRIPT_COMMANDS structure variables.
 * \return 0 (no error) Other (error)
 ***********************************************************/
int process_cw_qsk_right_channel(ScriptParsing *sp, SCRIPT_COMMANDS *sc)
{
	if(!sp)
		return script_function_parameter_error;

	bool value = sp->cw_qsk_right_channel();

	btnQSK2->value(value);
	btnQSK2->do_callback();

	return script_no_errors;
}

/** ********************************************************
 * \brief
 * \param sp Access to ScritpParsing members.
 * \param sc Access to SCRIPT_COMMANDS structure variables.
 * \return 0 (no error) Other (error)
 ***********************************************************/
int process_pseudo_fsk_right_channel(ScriptParsing *sp, SCRIPT_COMMANDS *sc)
{
	if(!sp)
		return script_function_parameter_error;

	bool value = sp->pseudo_fsk_right_channel();

	chkPseudoFSK2->value(value);
	chkPseudoFSK2->do_callback();

	return script_no_errors;
}

/** ********************************************************
 * \brief
 * \param sp Access to ScritpParsing members.
 * \param sc Access to SCRIPT_COMMANDS structure variables.
 * \return 0 (no error) Other (error)
 ***********************************************************/
int process_wave_file_sample_rate(ScriptParsing *sp, SCRIPT_COMMANDS *sc)
{
	if(!sp)
		return script_function_parameter_error;

	char buffer[32];
	int value = sp->wave_file_sample_rate();

	if((value < 8000) || (value > 48000))
		return script_invalid_parameter;

	memset(buffer, 0, sizeof(buffer));
	snprintf(buffer, sizeof(buffer) - 1, "%d", value);

	int index = listbox_wav_samplerate->find_index(buffer);
	if(!index)
		return script_invalid_parameter;

	listbox_wav_samplerate->index(index);
	listbox_wav_samplerate->do_callback();

	return script_no_errors;
}

/** ********************************************************
 * \brief
 * \param sp Access to ScritpParsing members.
 * \param sc Access to SCRIPT_COMMANDS structure variables.
 * \return 0 (no error) Other (error)
 ***********************************************************/
int process_hrdw_ptt_right_audio_channel(ScriptParsing *sp, SCRIPT_COMMANDS *sc)
{
	if(!sp)
		return script_function_parameter_error;

	bool value = sp->hrdw_ptt_right_audio_channel();

	btnPTTrightchannel->value(value);
	btnPTTrightchannel->do_callback();

	return script_no_errors;
}

/** ********************************************************
 * \brief
 * \param sp Access to ScritpParsing members.
 * \param sc Access to SCRIPT_COMMANDS structure variables.
 * \return 0 (no error) Other (error)
 ***********************************************************/
int process_hrdw_ptt_sep_serial_port(ScriptParsing *sp, SCRIPT_COMMANDS *sc)
{
	if(!sp)
		return script_function_parameter_error;

	bool value = sp->hrdw_ptt_sep_serial_port();

	btnTTYptt->value(value);
	btnTTYptt->do_callback();

	return script_no_errors;
}

/** ********************************************************
 * \brief
 * \param sp Access to ScritpParsing members.
 * \param sc Access to SCRIPT_COMMANDS structure variables.
 * \return 0 (no error) Other (error)
 * \par NOTE:
 * Fl_ComboBox (custom widget)
 * find_index() located in combo.cxx and used here returns -1 
 * on non-matching.
 ***********************************************************/
int process_hrdw_ptt_sep_serial_port_path(ScriptParsing *sp, SCRIPT_COMMANDS *sc)
{
	if(!sp)
		return script_function_parameter_error;

	std::string str_data;

	str_data.assign(sp->hrdw_ptt_sep_serial_port_path());
	if(str_data.empty())
		return script_invalid_parameter;

	int index = inpTTYdev->find_index(str_data.c_str());
	if(index < 0)
		return script_invalid_parameter;

	inpTTYdev->index(index);
	inpTTYdev->do_callback();

	return script_no_errors;
}

/** ********************************************************
 * \brief
 * \param sp Access to ScritpParsing members.
 * \param sc Access to SCRIPT_COMMANDS structure variables.
 * \return 0 (no error) Other (error)
 ***********************************************************/
int process_hrdw_ptt_sep_serial_port_rts(ScriptParsing *sp, SCRIPT_COMMANDS *sc)
{
	if(!sp)
		return script_function_parameter_error;

	bool value = sp->hrdw_ptt_sep_serial_port_rts();

	btnRTSptt->value(value);
	btnRTSptt->do_callback();

	return script_no_errors;
}

/** ********************************************************
 * \brief
 * \param sp Access to ScritpParsing members.
 * \param sc Access to SCRIPT_COMMANDS structure variables.
 * \return 0 (no error) Other (error)
 ***********************************************************/
int process_hrdw_ptt_sep_serial_port_dtr(ScriptParsing *sp, SCRIPT_COMMANDS *sc)
{
	if(!sp)
		return script_function_parameter_error;

	bool value = sp->hrdw_ptt_sep_serial_port_dtr();

	btnDTRptt->value(value);
	btnDTRptt->do_callback();

	return script_no_errors;
}

/** ********************************************************
 * \brief
 * \param sp Access to ScritpParsing members.
 * \param sc Access to SCRIPT_COMMANDS structure variables.
 * \return 0 (no error) Other (error)
 ***********************************************************/
int process_hrdw_ptt_sep_serial_port_rts_v(ScriptParsing *sp, SCRIPT_COMMANDS *sc)
{
	if(!sp)
		return script_function_parameter_error;

	bool value = sp->hrdw_ptt_sep_serial_port_rts_v();

	btnRTSplusV->value(value);
	btnRTSplusV->do_callback();

	return script_no_errors;
}

/** ********************************************************
 * \brief
 * \param sp Access to ScritpParsing members.
 * \param sc Access to SCRIPT_COMMANDS structure variables.
 * \return 0 (no error) Other (error)
 ***********************************************************/
int process_hrdw_ptt_sep_serial_port_dtr_v(ScriptParsing *sp, SCRIPT_COMMANDS *sc)
{
	if(!sp)
		return script_function_parameter_error;

	bool value = sp->hrdw_ptt_sep_serial_port_dtr_v();

	btnDTRplusV->value(value);
	btnDTRplusV->do_callback();

	return script_no_errors;
}

/** ********************************************************
 * \brief
 * \param sp Access to ScritpParsing members.
 * \param sc Access to SCRIPT_COMMANDS structure variables.
 * \return 0 (no error) Other (error)
 ***********************************************************/
int process_hrdw_ptt_start_delay(ScriptParsing *sp, SCRIPT_COMMANDS *sc)
{
	if(!sp)
		return script_function_parameter_error;

	int value = sp->hrdw_ptt_start_delay();

	cntPTT_on_delay->value(value);
	cntPTT_on_delay->do_callback();

	return script_no_errors;
}

/** ********************************************************
 * \brief
 * \param sp Access to ScritpParsing members.
 * \param sc Access to SCRIPT_COMMANDS structure variables.
 * \return 0 (no error) Other (error)
 ***********************************************************/
int process_hrdw_ptt_end_delay(ScriptParsing *sp, SCRIPT_COMMANDS *sc)
{
	if(!sp)
		return script_function_parameter_error;

	int value = sp->hrdw_ptt_end_delay();

	cntPTT_off_delay->value(value);
	cntPTT_off_delay->do_callback();

	return script_no_errors;
}

/** ********************************************************
 * \brief
 * \param sp Access to ScritpParsing members.
 * \param sc Access to SCRIPT_COMMANDS structure variables.
 * \return 0 (no error) Other (error)
 ***********************************************************/
int process_hrdw_ptt_uhrouter(ScriptParsing *sp, SCRIPT_COMMANDS *sc)
{
	if(!sp)
		return script_function_parameter_error;

	bool value = sp->hrdw_ptt_uhrouter();

	btnUseUHrouterPTT->value(value);
	btnUseUHrouterPTT->do_callback();

	return script_no_errors;
}

/** ********************************************************
 * \brief
 * \param sp Access to ScritpParsing members.
 * \param sc Access to SCRIPT_COMMANDS structure variables.
 * \return 0 (no error) Other (error)
 ***********************************************************/
int process_hrdw_ptt_parallel(ScriptParsing *sp, SCRIPT_COMMANDS *sc)
{
	if(!sp)
		return script_function_parameter_error;

	bool value = sp->hrdw_ptt_parallel();

	btnUsePPortPTT->value(value);
	btnUsePPortPTT->do_callback();

	return script_no_errors;
}

/** ********************************************************
 * \brief
 * \param sp Access to ScritpParsing members.
 * \param sc Access to SCRIPT_COMMANDS structure variables.
 * \return 0 (no error) Other (error)
 ***********************************************************/
int process_hrdw_ptt_initialize(ScriptParsing *sp, SCRIPT_COMMANDS *sc)
{
	if(!sp)
		return script_function_parameter_error;

	bool value = sp->hrdw_ptt_initialize();

	if(value)
		btnInitHWPTT->do_callback();

	return script_no_errors;
}

/** ********************************************************
 * \brief
 * \param sp Access to ScritpParsing members.
 * \param sc Access to SCRIPT_COMMANDS structure variables.
 * \return 0 (no error) Other (error)
 ***********************************************************/
int process_use_rigcat(ScriptParsing *sp, SCRIPT_COMMANDS *sc)
{
	if(!sp)
		return script_function_parameter_error;

	bool value = sp->use_rigcat();

	chkUSERIGCAT->value(value);
	chkUSERIGCAT->do_callback();

	return script_no_errors;
}

extern void loadRigXmlFile(void);
/** ********************************************************
 * \brief
 * \param sp Access to ScritpParsing members.
 * \param sc Access to SCRIPT_COMMANDS structure variables.
 * \return 0 (no error) Other (error)
 ***********************************************************/
int process_rigcat_desc_file(ScriptParsing *sp, SCRIPT_COMMANDS *sc)
{
	if(!sp)
		return script_function_parameter_error;

	std::string str_data;

	str_data.assign(sp->rigcat_desc_file());
	if(str_data.empty())
		return script_invalid_parameter;

	progdefaults.XmlRigFilename.assign(str_data);
	txtXmlRigFilename->value(fl_filename_name(progdefaults.XmlRigFilename.c_str()));
	loadRigXmlFile();

	return script_no_errors;
}

/** ********************************************************
 * \brief
 * \param sp Access to ScritpParsing members.
 * \param sc Access to SCRIPT_COMMANDS structure variables.
 * \return 0 (no error) Other (error)
 * \par NOTE:
 * Fl_ComboBox (custom widget)
 * find_index() located in combo.cxx and used here returns -1
 * on non-matching.
 ***********************************************************/
int process_rigcat_device_path(ScriptParsing *sp, SCRIPT_COMMANDS *sc)
{
	if(!sp)
		return script_function_parameter_error;

	std::string str_data;

	str_data.assign(sp->rigcat_device_path());
	if(str_data.empty())
		return script_invalid_parameter;

	int index = inpXmlRigDevice->find_index((char *)str_data.c_str());
	if(index < 0)
		return script_invalid_parameter;

	inpXmlRigDevice->index(index);
	inpXmlRigDevice->do_callback();

	return script_no_errors;
}

/** ********************************************************
 * \brief
 * \param sp Access to ScritpParsing members.
 * \param sc Access to SCRIPT_COMMANDS structure variables.
 * \return 0 (no error) Other (error)
 ***********************************************************/
int process_rigcat_retries(ScriptParsing *sp, SCRIPT_COMMANDS *sc)
{
	if(!sp)
		return script_function_parameter_error;

	int value = sp->rigcat_retries();

	progdefaults.RigCatRetries = value;
	progdefaults.changed = true;

	cntRigCatRetries->value(value);
	cntRigCatRetries->do_callback();

	return script_no_errors;
}

/** ********************************************************
 * \brief
 * \param sp Access to ScritpParsing members.
 * \param sc Access to SCRIPT_COMMANDS structure variables.
 * \return 0 (no error) Other (error)
 ***********************************************************/
int process_rigcat_retry_interval(ScriptParsing *sp, SCRIPT_COMMANDS *sc)
{
	if(!sp)
		return script_function_parameter_error;

	int value = sp->rigcat_retry_interval();

	progdefaults.RigCatTimeout = value;
	progdefaults.changed = true;

	cntRigCatTimeout->value(value);
	cntRigCatTimeout->do_callback();

	return script_no_errors;
}

/** ********************************************************
 * \brief
 * \param sp Access to ScritpParsing members.
 * \param sc Access to SCRIPT_COMMANDS structure variables.
 * \return 0 (no error) Other (error)
 ***********************************************************/
int process_rigcat_write_delay(ScriptParsing *sp, SCRIPT_COMMANDS *sc)
{
	if(!sp)
		return script_function_parameter_error;

	int value = sp->rigcat_write_delay();

	progdefaults.RigCatTimeout = value;
	progdefaults.changed = true;

	cntRigCatWait->value(value);
	cntRigCatWait->do_callback();

	return script_no_errors;
}

/** ********************************************************
 * \brief
 * \param sp Access to ScritpParsing members.
 * \param sc Access to SCRIPT_COMMANDS structure variables.
 * \return 0 (no error) Other (error)
 ***********************************************************/
int process_rigcat_init_delay(ScriptParsing *sp, SCRIPT_COMMANDS *sc)
{
	if(!sp)
		return script_function_parameter_error;

	int value = sp->rigcat_init_delay();

	progdefaults.RigCatInitDelay = value;
	progdefaults.changed = true;

	cntRigCatInitDelay->value(value);
	cntRigCatInitDelay->do_callback();

	return script_no_errors;
}

/** ********************************************************
 * \brief
 * \param sp Access to ScritpParsing members.
 * \param sc Access to SCRIPT_COMMANDS structure variables.
 * \return 0 (no error) Other (error)
 ***********************************************************/
int process_rigcat_baud_rate(ScriptParsing *sp, SCRIPT_COMMANDS *sc)
{
	if(!sp)
		return script_function_parameter_error;

	std::string str_data;

	str_data.assign(sp->rigcat_baud_rate());
	if(str_data.empty())
		return script_invalid_parameter;

	int index = listbox_xml_rig_baudrate->find_index((char *)str_data.c_str());
	if(index < 0)
		return script_invalid_parameter;

	progdefaults.XmlRigBaudrate = index;
	progdefaults.changed = true;

	listbox_xml_rig_baudrate->index(index);
	listbox_xml_rig_baudrate->do_callback();

	return script_no_errors;
}

/** ********************************************************
 * \brief
 * \param sp Access to ScritpParsing members.
 * \param sc Access to SCRIPT_COMMANDS structure variables.
 * \return 0 (no error) Other (error)
 ***********************************************************/
int process_rigcat_cat_command_ptt(ScriptParsing *sp, SCRIPT_COMMANDS *sc)
{
	if(!sp)
		return script_function_parameter_error;

	bool value = sp->rigcat_cat_command_ptt();

	progdefaults.RigCatCMDptt = value;
	progdefaults.changed = true;

	btnRigCatCMDptt->value(value);
	btnRigCatCMDptt->do_callback();

	return script_no_errors;
}

/** ********************************************************
 * \brief
 * \param sp Access to ScritpParsing members.
 * \param sc Access to SCRIPT_COMMANDS structure variables.
 * \return 0 (no error) Other (error)
 ***********************************************************/
int process_rigcat_stop_bits(ScriptParsing *sp, SCRIPT_COMMANDS *sc)
{
	if(!sp)
		return script_function_parameter_error;

	std::string str_data;

	str_data.assign(sp->rigcat_stop_bits());
	if(str_data.empty())
		return script_invalid_parameter;

	int value = 0;

	if(sscanf(str_data.c_str(), "%d", &value) != 1)
		return script_invalid_parameter;

	if(value < ((int)valRigCatStopbits->minimum()) ||
		(value > ((int)valRigCatStopbits->maximum())))
		return script_invalid_parameter;

	progdefaults.RigCatStopbits = value;
	progdefaults.changed = true;

	valRigCatStopbits->value(value);
	valRigCatStopbits->do_callback();

	return script_no_errors;
}

/** ********************************************************
 * \brief
 * \param sp Access to ScritpParsing members.
 * \param sc Access to SCRIPT_COMMANDS structure variables.
 * \return 0 (no error) Other (error)
 ***********************************************************/
int process_rigcat_commands_echoed(ScriptParsing *sp, SCRIPT_COMMANDS *sc)
{
	if(!sp)
		return script_function_parameter_error;

	bool value = sp->rigcat_commands_echoed();

	progdefaults.RigCatECHO = value;
	progdefaults.changed = true;

	btnRigCatEcho->value(value);
	btnRigCatEcho->do_callback();

	return script_no_errors;
}

/** ********************************************************
 * \brief
 * \param sp Access to ScritpParsing members.
 * \param sc Access to SCRIPT_COMMANDS structure variables.
 * \return 0 (no error) Other (error)
 ***********************************************************/
int process_rigcat_toggle_rts_ptt(ScriptParsing *sp, SCRIPT_COMMANDS *sc)
{
	if(!sp)
		return script_function_parameter_error;

	bool value = sp->rigcat_toggle_rts_ptt();

	progdefaults.RigCatRTSptt = value;
	progdefaults.changed = true;

	btnRigCatRTSptt->value(value);
	btnRigCatRTSptt->do_callback();

	return script_no_errors;
}

/** ********************************************************
 * \brief
 * \param sp Access to ScritpParsing members.
 * \param sc Access to SCRIPT_COMMANDS structure variables.
 * \return 0 (no error) Other (error)
 ***********************************************************/
int process_rigcat_restore_on_close(ScriptParsing *sp, SCRIPT_COMMANDS *sc)
{
	if(!sp)
		return script_function_parameter_error;

	bool value = sp->rigcat_restore_on_close();

	progdefaults.RigCatRestoreTIO = value;
	progdefaults.changed = true;

	chk_restore_tio->value(value);
	chk_restore_tio->do_callback();

	return script_no_errors;
}

/** ********************************************************
 * \brief
 * \param sp Access to ScritpParsing members.
 * \param sc Access to SCRIPT_COMMANDS structure variables.
 * \return 0 (no error) Other (error)
 ***********************************************************/
int process_rigcat_rts_12v(ScriptParsing *sp, SCRIPT_COMMANDS *sc)
{
	if(!sp)
		return script_function_parameter_error;

	bool value = sp->rigcat_rts_12v();

	progdefaults.RigCatRTSplus = value;
	progdefaults.changed = true;

	btnRigCatRTSplus->value(value);
	btnRigCatRTSplus->do_callback();

	return script_no_errors;
}
/** ********************************************************
 * \brief
 * \param sp Access to ScritpParsing members.
 * \param sc Access to SCRIPT_COMMANDS structure variables.
 * \return 0 (no error) Other (error)
 ***********************************************************/
int process_rigcat_dtr_12v(ScriptParsing *sp, SCRIPT_COMMANDS *sc)
{
	if(!sp)
		return script_function_parameter_error;

	bool value = sp->rigcat_dtr_12v();

	progdefaults.RigCatDTRplus = value;
	progdefaults.changed = true;

	btnRigCatDTRplus->value(value);
	btnRigCatDTRplus->do_callback();

	return script_no_errors;
}

/** ********************************************************
 * \brief
 * \param sp Access to ScritpParsing members.
 * \param sc Access to SCRIPT_COMMANDS structure variables.
 * \return 0 (no error) Other (error)
 ***********************************************************/
int process_rigcat_hrdwr_flow(ScriptParsing *sp, SCRIPT_COMMANDS *sc)
{
	if(!sp)
		return script_function_parameter_error;

	bool value = sp->rigcat_hrdwr_flow();

	progdefaults.RigCatRTSCTSflow = value;
	progdefaults.changed = true;

	chkRigCatRTSCTSflow->value(value);
	chkRigCatRTSCTSflow->do_callback();

	return script_no_errors;
}

/** ********************************************************
 * \brief
 * \param sp Access to ScritpParsing members.
 * \param sc Access to SCRIPT_COMMANDS structure variables.
 * \return 0 (no error) Other (error)
 ***********************************************************/
int process_rigcat_vsp_enable(ScriptParsing *sp, SCRIPT_COMMANDS *sc)
{
	if(!sp)
		return script_function_parameter_error;

	bool value = sp->rigcat_hrdwr_flow();

	progdefaults.RigCatVSP = value;
	progdefaults.changed = true;

	chkRigCatVSP->value(value);
	chkRigCatVSP->do_callback();

	return script_no_errors;
}

/** ********************************************************
 * \brief
 * \param sp Access to ScritpParsing members.
 * \param sc Access to SCRIPT_COMMANDS structure variables.
 * \return 0 (no error) Other (error)
 ***********************************************************/
int process_rigcat_initialize(ScriptParsing *sp, SCRIPT_COMMANDS *sc)
{
	if(!sp)
		return script_function_parameter_error;

	bool value = sp->rigcat_initialize();

	if(value)
		btnInitRIGCAT->do_callback();

	return script_no_errors;
}

/** ********************************************************
 * \brief
 * \param sp Access to ScritpParsing members.
 * \param sc Access to SCRIPT_COMMANDS structure variables.
 * \return 0 (no error) Other (error)
 ***********************************************************/
int process_use_hamlib(ScriptParsing *sp, SCRIPT_COMMANDS *sc)
{
#if USE_HAMLIB
	if(!sp)
		return script_function_parameter_error;

	bool value = sp->use_hamlib();

	chkUSEHAMLIB->value(value);
	chkUSEHAMLIB->do_callback();

#endif // USE_HAMLIB

	return script_no_errors;
}

/** ********************************************************
 * \brief
 * \param sp Access to ScritpParsing members.
 * \param sc Access to SCRIPT_COMMANDS structure variables.
 * \return 0 (no error) Other (error)
 ***********************************************************/
int process_hamlib_rig(ScriptParsing *sp, SCRIPT_COMMANDS *sc)
{
#if USE_HAMLIB
	if(!sp)
		return script_function_parameter_error;

	std::string str_data;

	str_data.assign(sp->hamlib_rig());
	if(str_data.empty())
		return script_invalid_parameter;

	int index = cboHamlibRig->find_index(str_data.c_str());
	if(index < 0)
		return script_invalid_parameter;

	cboHamlibRig->index(index);
	cboHamlibRig->do_callback();
#endif // USE_HAMLIB

	return script_no_errors;
}

/** ********************************************************
 * \brief
 * \param sp Access to ScritpParsing members.
 * \param sc Access to SCRIPT_COMMANDS structure variables.
 * \return 0 (no error) Other (error)
 ***********************************************************/
int process_hamlib_retries(ScriptParsing *sp, SCRIPT_COMMANDS *sc)
{
#if USE_HAMLIB
	if(!sp)
		return script_function_parameter_error;

	int value = sp->hamlib_retries();

	progdefaults.HamlibRetries = value;
	progdefaults.changed = true;

	cntHamlibRetries->value(value);
	cntHamlibRetries->do_callback();

#endif // USE_HAMLIB
	return script_no_errors;
}

/** ********************************************************
 * \brief
 * \param sp Access to ScritpParsing members.
 * \param sc Access to SCRIPT_COMMANDS structure variables.
 * \return 0 (no error) Other (error)
 ***********************************************************/
int process_hamlib_retry_interval(ScriptParsing *sp, SCRIPT_COMMANDS *sc)
{
#if USE_HAMLIB
	if(!sp)
		return script_function_parameter_error;

	int value = sp->hamlib_retry_interval();

	progdefaults.HamlibTimeout = value;
	progdefaults.changed = true;

	cntHamlibTimeout->value(value);
	cntHamlibTimeout->do_callback();

#endif // USE_HAMLIB
	return script_no_errors;
}

/** ********************************************************
 * \brief
 * \param sp Access to ScritpParsing members.
 * \param sc Access to SCRIPT_COMMANDS structure variables.
 * \return 0 (no error) Other (error)
 ***********************************************************/
int process_hamlib_write_delay(ScriptParsing *sp, SCRIPT_COMMANDS *sc)
{
#if USE_HAMLIB
	if(!sp)
		return script_function_parameter_error;

	int value = sp->hamlib_write_delay();

	progdefaults.HamlibWriteDelay = value;
	progdefaults.changed = true;

	cntHamlibWriteDelay->value(value);
	cntHamlibWriteDelay->do_callback();

#endif // USE_HAMLIB
	return script_no_errors;
}

/** ********************************************************
 * \brief
 * \param sp Access to ScritpParsing members.
 * \param sc Access to SCRIPT_COMMANDS structure variables.
 * \return 0 (no error) Other (error)
 ***********************************************************/
int process_hamlib_post_write_delay(ScriptParsing *sp, SCRIPT_COMMANDS *sc)
{
#if USE_HAMLIB
	if(!sp)
		return script_function_parameter_error;

	int value = sp->hamlib_post_write_delay();

	progdefaults.HamlibWait = value;
	progdefaults.changed = true;

	cntHamlibWait->value(value);
	cntHamlibWait->do_callback();

#endif // USE_HAMLIB
	return script_no_errors;
}

/** ********************************************************
 * \brief
 * \param sp Access to ScritpParsing members.
 * \param sc Access to SCRIPT_COMMANDS structure variables.
 * \return 0 (no error) Other (error)
 * \par Note:
 * This string storage can be assigned to anything. User
 * should follow the limitations imposed by the rules
 * of the host country.
 ***********************************************************/
int process_hamlib_device_path(ScriptParsing *sp, SCRIPT_COMMANDS *sc)
{
#if USE_HAMLIB

	if(!sp)
		return script_function_parameter_error;

	std::string str_data;

	str_data.assign(sp->hamlib_device_path());
	if(str_data.empty())
		return script_invalid_parameter;

	int index = cboHamlibRig->find_index(str_data.c_str());
	if(index < 0)
		return script_invalid_parameter;

	progdefaults.HamRigDevice.assign(str_data);
	progdefaults.changed = true;

	inpRIGdev->value(str_data.c_str());
	inpRIGdev->do_callback();

#endif // USE_HAMLIB

	return script_no_errors;
}

/** ********************************************************
 * \brief
 * \param sp Access to ScritpParsing members.
 * \param sc Access to SCRIPT_COMMANDS structure variables.
 * \return 0 (no error) Other (error)
 ***********************************************************/
int process_hamlib_baud_rate(ScriptParsing *sp, SCRIPT_COMMANDS *sc)
{
#if USE_HAMLIB

	if(!sp)
		return script_function_parameter_error;

	std::string str_data;

	str_data.assign(sp->hamlib_baud_rate());
	if(str_data.empty())
		return script_invalid_parameter;

	int index = listbox_baudrate->find_index(str_data.c_str());
	if(index < 0)
		return script_invalid_parameter;

	progdefaults.HamRigBaudrate = index;
	progdefaults.changed = true;

	listbox_baudrate->index(index);
	listbox_baudrate->do_callback();

#endif // USE_HAMLIB
	return script_no_errors;
}

/** ********************************************************
 * \brief
 * \param sp Access to ScritpParsing members.
 * \param sc Access to SCRIPT_COMMANDS structure variables.
 * \return 0 (no error) Other (error)
 ***********************************************************/
int process_hamlib_stop_bits(ScriptParsing *sp, SCRIPT_COMMANDS *sc)
{
#if USE_HAMLIB

	if(!sp)
		return script_function_parameter_error;

	std::string str_data;
	str_data.assign(sp->hamlib_stop_bits());

	if(str_data.empty())
		return script_invalid_parameter;

	int value = 0;
	sscanf(str_data.c_str(), "%d", &value);

	if(value < ((int)valHamRigStopbits->minimum()) ||
		(value > ((int)valHamRigStopbits->maximum())))
			return script_invalid_parameter;

	progdefaults.HamRigStopbits = value;
	progdefaults.changed = true;

	valHamRigStopbits->value(value);
	valHamRigStopbits->do_callback();

#endif // USE_HAMLIB
	return script_no_errors;
}

/** ********************************************************
 * \brief
 * \param sp Access to ScritpParsing members.
 * \param sc Access to SCRIPT_COMMANDS structure variables.
 * \return 0 (no error) Other (error)
 ***********************************************************/
int process_hamlib_sideband(ScriptParsing *sp, SCRIPT_COMMANDS *sc)
{
#if USE_HAMLIB

	if(!sp)
		return script_function_parameter_error;

	std::string str_data;

	str_data.assign(sp->hamlib_sideband());
	if(str_data.empty())
		return script_invalid_parameter;

	int index = listbox_sideband->find_index(str_data.c_str());
	if(index < 0)
		return script_invalid_parameter;

	progdefaults.HamlibSideband = index;
	progdefaults.changed = true;

	listbox_sideband->index(index);
	listbox_sideband->do_callback();

#endif // USE_HAMLIB
	return script_no_errors;
}

/** ********************************************************
 * \brief
 * \param sp Access to ScritpParsing members.
 * \param sc Access to SCRIPT_COMMANDS structure variables.
 * \return 0 (no error) Other (error)
 ***********************************************************/
int process_hamlib_ptt_hl_command(ScriptParsing *sp, SCRIPT_COMMANDS *sc)
{
#if USE_HAMLIB
	if(!sp)
		return script_function_parameter_error;

	bool value = sp->hamlib_ptt_hl_command();

	progdefaults.HamlibCMDptt = value;
	progdefaults.changed = true;

	btnHamlibCMDptt->value(value);
	btnHamlibCMDptt->do_callback();

#endif // USE_HAMLIB

	return script_no_errors;
}

/** ********************************************************
 * \brief
 * \param sp Access to ScritpParsing members.
 * \param sc Access to SCRIPT_COMMANDS structure variables.
 * \return 0 (no error) Other (error)
 ***********************************************************/
int process_hamlib_dtr_12(ScriptParsing *sp, SCRIPT_COMMANDS *sc)
{
#if USE_HAMLIB
	if(!sp)
		return script_function_parameter_error;

	bool value = sp->hamlib_dtr_12();

	progdefaults.HamlibDTRplus = value;
	progdefaults.changed = true;

	btnHamlibDTRplus->value(value);
	btnHamlibDTRplus->do_callback();

#endif // USE_HAMLIB

	return script_no_errors;
}

/** ********************************************************
 * \brief
 * \param sp Access to ScritpParsing members.
 * \param sc Access to SCRIPT_COMMANDS structure variables.
 * \return 0 (no error) Other (error)
 ***********************************************************/
int process_hamlib_rts_12(ScriptParsing *sp, SCRIPT_COMMANDS *sc)
{
#if USE_HAMLIB
	if(!sp)
		return script_function_parameter_error;

	bool value = sp->hamlib_rts_12();

	progdefaults.HamlibRTSplus = value;
	progdefaults.changed = true;

	chkHamlibRTSplus->value(value);
	chkHamlibRTSplus->do_callback();

#endif // USE_HAMLIB

	return script_no_errors;
}

/** ********************************************************
 * \brief
 * \param sp Access to ScritpParsing members.
 * \param sc Access to SCRIPT_COMMANDS structure variables.
 * \return 0 (no error) Other (error)
 ***********************************************************/
int process_hamlib_rts_cts_flow(ScriptParsing *sp, SCRIPT_COMMANDS *sc)
{
#if USE_HAMLIB
	if(!sp)
		return script_function_parameter_error;

	bool value = sp->hamlib_rts_cts_flow();

	progdefaults.HamlibRTSCTSflow = value;
	progdefaults.changed = true;

	chkHamlibRTSCTSflow->value(value);
	chkHamlibRTSCTSflow->do_callback();

#endif // USE_HAMLIB

	return script_no_errors;
}

/** ********************************************************
 * \brief
 * \param sp Access to ScritpParsing members.
 * \param sc Access to SCRIPT_COMMANDS structure variables.
 * \return 0 (no error) Other (error)
 ***********************************************************/
int process_hamlib_xon_xoff_flow(ScriptParsing *sp, SCRIPT_COMMANDS *sc)
{
#if USE_HAMLIB
	if(!sp)
		return script_function_parameter_error;

	bool value = sp->hamlib_rts_cts_flow();

	progdefaults.HamlibXONXOFFflow = value;
	progdefaults.changed = true;

	chkHamlibXONXOFFflow->value(value);
	chkHamlibXONXOFFflow->do_callback();

#endif // USE_HAMLIB

	return script_no_errors;
}

/** ********************************************************
 * \brief
 * \param sp Access to ScritpParsing members.
 * \param sc Access to SCRIPT_COMMANDS structure variables.
 * \return 0 (no error) Other (error)
 ***********************************************************/
int process_hamlib_advanced_config(ScriptParsing *sp, SCRIPT_COMMANDS *sc)
{
#if USE_HAMLIB
	if(!sp)
		return script_function_parameter_error;

	std::string value = sp->hamlib_advanced_config();

	if(value.empty())
		return script_invalid_parameter;

	progdefaults.HamConfig.assign(value);
	progdefaults.changed = true;

	inpHamlibConfig->value(value.c_str());
	inpHamlibConfig->do_callback();

#endif // USE_HAMLIB

	return script_no_errors;
}

/** ********************************************************
 * \brief
 * \param sp Access to ScritpParsing members.
 * \param sc Access to SCRIPT_COMMANDS structure variables.
 * \return 0 (no error) Other (error)
 ***********************************************************/
int process_hamlib_initialize(ScriptParsing *sp, SCRIPT_COMMANDS *sc)
{
#if USE_HAMLIB
	if(!sp)
		return script_function_parameter_error;

	bool value = sp->hamlib_initialize();

	if(value)
 		inpHamlibConfig->do_callback();

#endif // USE_HAMLIB

	return script_no_errors;
}

/** ********************************************************
 * \brief
 * \param sp Access to ScritpParsing members.
 * \param sc Access to SCRIPT_COMMANDS structure variables.
 * \return 0 (no error) Other (error)
 ***********************************************************/
int process_use_xml_rpc(ScriptParsing *sp, SCRIPT_COMMANDS *sc)
{
	if(!sp)
		return script_function_parameter_error;

	bool value = sp->use_xml_rpc();

	progdefaults.chkUSEXMLRPCis = value;
	progdefaults.changed = true;

	chkUSEXMLRPC->value(value);
	chkUSEXMLRPC->do_callback();

	return script_no_errors;
}

/** ********************************************************
 * \brief
 * \param sp Access to ScritpParsing members.
 * \param sc Access to SCRIPT_COMMANDS structure variables.
 * \return 0 (no error) Other (error)
 ***********************************************************/
int process_xml_rpc_mode_bw_delay(ScriptParsing *sp, SCRIPT_COMMANDS *sc)
{
	if(!sp)
		return script_function_parameter_error;

	std::string str = sp->xml_rpc_mode_bw_delay();
	if(str.empty())
		return script_invalid_parameter;

	double value = 0.0;
	int count = sscanf(str.c_str(), "%f", &value);

	if(count < 1)
		return script_invalid_parameter;

	if(value < ((double) mbw_delay->minimum()) ||
		(value > ((double) mbw_delay->maximum())))
			return script_invalid_parameter;

	progdefaults.mbw = value;
	progdefaults.changed = true;

	mbw_delay->value(value);
	mbw_delay->do_callback();

	return script_no_errors;
}

/** ********************************************************
 * \brief
 * \param sp Access to ScritpParsing members.
 * \param sc Access to SCRIPT_COMMANDS structure variables.
 * \return 0 (no error) Other (error)
 ***********************************************************/
int process_xml_rpc_initialize(ScriptParsing *sp, SCRIPT_COMMANDS *sc)
{
	if(!sp)
		return script_function_parameter_error;

	bool value = sp->xml_rpc_initialize();

	if(value)
		btnInitXMLRPC->do_callback();

	return script_no_errors;
}

/** ********************************************************
 * \brief Reset Configuration panel attributes
 * \param sp Access to ScritpParsing members.
 * \param sc Access to SCRIPT_COMMANDS structure variables.
 * \return 0 (no error) Other (error)
 ***********************************************************/
int process_reset(ScriptParsing *sp, SCRIPT_COMMANDS *sc)
{
	return script_no_errors;
}

/** ********************************************************
 * \brief Call back function when executing a configuration script.
 * Called from the File->Execute Config Script menu item.
 * \param void
 ***********************************************************/
void script_execute(const char *filename, bool queue_flag)
{
	int count = 0;
	int index = 0;
	SCRIPT_CODES error = script_no_errors;
	ScriptParsing *sp = 0;
	static std::string script_filename = "";

	if(!filename) {
		LOG_INFO(_("Script file name (path) null pointer"));
		return;
	}

	script_filename.assign(filename);

	if(script_filename.empty()) {
		LOG_INFO(_("Script file name (path) invalid"));
		return;
	}

	sp = new ScriptParsing;

	if(!sp) {
		LOG_INFO(_("ScriptParsing Class Allocation Fail (%s)"), script_filename.c_str());
		return;
	}

	// LOG_INFO("Executing Script:%s", script_filename.c_str());

	error = sp->parse_commands((char *) script_filename.c_str());

	if(error != script_no_errors) {
		LOG_INFO(_("Error(s) in processing script file: %s"), script_filename.c_str());
		fl_alert("%s", _("Script File contains Error(s)\nSee Log file for details."));
	}

	if(sp)
		delete sp;
}

/** ********************************************************
 * \brief Call back function when executing a configuration script.
 * Called from the File->Execute Config Script menu item.
 * \param void
 ***********************************************************/
void cb_scripts(bool reset_path = false)
{
	pthread_mutex_lock(&mutex_script_io);

	static bool first_time = true;
	static char script_filename[FL_PATH_MAX + 1];
	std::string new_path = "";

	if(reset_path || first_time) {
		memset(script_filename, 0, sizeof(script_filename));
		strncpy(script_filename, ScriptsDir.c_str(), FL_PATH_MAX);
		int len = strnlen(script_filename, FL_PATH_MAX);

		if(len > 0) {
			len--;
			if(script_filename[len] == PATH_CHAR_SEPERATOR);
			else strncat(script_filename, PATH_SEPERATOR, FL_PATH_MAX);
		} else {
			return;
		}

		first_time = false;
	}

	const char *p = FSEL::select((char *)_("Script Files"), (char *)_("*.txt"), \
		script_filename);

	if(p) {
		memset(script_filename, 0, sizeof(script_filename));
		strncpy(script_filename, p, FL_PATH_MAX);

		Fl::lock();
		script_execute(script_filename, false);
		Fl::unlock();

	}

	pthread_mutex_unlock(&mutex_script_io);
}
