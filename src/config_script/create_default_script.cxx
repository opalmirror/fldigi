/** **************************************************************
 \page create_default_script Default script generator

 \par create_default_script.cxx (FLDIGI)

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
	Used to generate a config script  from the current settings.
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

void cb_create_default_script(void);
int create_default_script(char *file_name);

static int add_list_command(FILE *fd, char *cmd);
static int add_command(FILE *fd, char *cmd, char *param, size_t size);
static int add_sub_command(FILE *fd, char *cmd, char *param, size_t size);
static int copy_string(char *dst, const char *src, size_t size);
static int copy_bool(char *dst, bool src, size_t size);
static int copy_int(char *dst, int src, size_t size);
static int copy_double(char *dst, double src, size_t size);

static int str_n_len(const char *src, size_t size);

extern pthread_mutex_t mutex_script_io;

/** ********************************************************
 * \brief Menu callback. Create default script based on the 
 * current settings.
 * \param none
 * \return void
 ***********************************************************/
void cb_create_default_script(void)
{
	pthread_mutex_lock(&mutex_script_io);

	static bool first_time = true;
	static char script_filename[FL_PATH_MAX + 1];
	std::string new_path = "";

	if(first_time) {
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

	const char *p = FSEL::saveas((char *)_("Script Files"), (char *)_("*.txt"), \
								 script_filename);

	if(p) {
		memset(script_filename, 0, sizeof(script_filename));
		strncpy(script_filename, p, FL_PATH_MAX);

		Fl::lock();
		create_default_script(script_filename);
		Fl::unlock();

	}

	pthread_mutex_unlock(&mutex_script_io);
}

/** ********************************************************
 * \brief Create default script based on thecurrent settings.
 * \param file_name Pointer to the file name and path.
 * \return 0 OK, other Error
 ***********************************************************/
int create_default_script(char *file_name)
{
	FILE *fd = (FILE *)0;
	static char buffer[FL_PATH_MAX];

	if(!file_name) {
		LOG_INFO(_("Invalid File Name Pointer (NULL) in function %s:%d"), __FILE__, __LINE__);
		return -1;
	}

	fd = fopen(file_name, "w");

	if(!fd) {
		LOG_INFO(_("Unable to create file %s (Error No=%d) func %s:%d"), file_name, errno, __FILE__, __LINE__);
		return -1;
	}

	memset(buffer, 0, sizeof(buffer));

// Tag the text file as a FLDIGI script file
	fprintf(fd, _("%s\n# Fldigi Generated Config Script\n"), SCRIPT_FILE_TAG);
	time_t thetime = time(0);
	fprintf(fd, _("# Created: %s\n"), ctime(&thetime));

// OPERATOR
	if(add_list_command(fd, (char *) CMD_OPERATOR))
		return fclose(fd);

	if(!copy_string(buffer, inpMyCallsign->value(), sizeof(buffer)))
		if(add_sub_command(fd, (char *)CMD_CALLSIGN, buffer, sizeof(buffer)))
			return fclose(fd);

	if(!copy_string(buffer, inpMyName->value(), sizeof(buffer)))
		if(add_sub_command(fd, (char *)CMD_QTH, buffer, sizeof(buffer)))
			return fclose(fd);

	if(!copy_string(buffer, inpMyQth->value(), sizeof(buffer)))
		if(add_sub_command(fd, (char *)CMD_NAME, buffer, sizeof(buffer)))
			return fclose(fd);

	if(!copy_string(buffer, inpMyLocator->value(), sizeof(buffer)))
		if(add_sub_command(fd, (char *)CMD_LOCATOR, buffer, sizeof(buffer)))
			return fclose(fd);

	if(!copy_string(buffer, inpMyAntenna->value(), sizeof(buffer)))
		if(add_sub_command(fd, (char *)CMD_ANTENNA, buffer, sizeof(buffer)))
			return fclose(fd);

	if(add_list_command(fd, (char *) CMD_OPERATOR_END))
		return fclose(fd);

// AUDIO DEVICE
	if(add_list_command(fd, (char *) CMD_AUDIO_DEVICE))
		return fclose(fd);

#if USE_OSS
	// OSS
	if(!copy_bool(buffer, btnAudioIO[0]->value(), sizeof(buffer)))
		if(add_sub_command(fd, (char *)CMD_OSS_AUDIO, buffer, sizeof(buffer)))
			return fclose(fd);

	if(!copy_string(buffer, menuOSSDev->value(), sizeof(buffer)))
		if(add_sub_command(fd, (char *)CMD_OSS_AUDIO_DEV_PATH, buffer, sizeof(buffer)))
			return fclose(fd);
#endif // USE_OSS

#if USE_PORTAUDIO
	// PORT AUDIO
	if(!copy_bool(buffer, btnAudioIO[1]->value(), sizeof(buffer)))
		if(add_sub_command(fd, (char *)CMD_PORT_AUDIO, buffer, sizeof(buffer)))
			return fclose(fd);

	if(!copy_string(buffer, menuPortInDev->text(), sizeof(buffer)))
		if(add_sub_command(fd, (char *)CMD_PORTA_CAP, buffer, sizeof(buffer)))
			return fclose(fd);

	if(!copy_string(buffer, menuPortOutDev->text(), sizeof(buffer)))
		if(add_sub_command(fd, (char *)CMD_PORTA_PLAY, buffer, sizeof(buffer)))
			return fclose(fd);
#endif // USE_PORTAUDIO

#if USE_PULSEAUDIO
	// PULSE AUDIO
	if(!copy_bool(buffer, btnAudioIO[2]->value(), sizeof(buffer)))
		if(add_sub_command(fd, (char *)CMD_PULSEA, buffer, sizeof(buffer)))
			return fclose(fd);

	if(!copy_string(buffer, inpPulseServer->value(), sizeof(buffer)))
		if(add_sub_command(fd, (char *)CMD_PULSEA_SERVER, buffer, sizeof(buffer)))
			return fclose(fd);

#endif // USE_PULSEAUDIO

	if(add_list_command(fd, (char *) CMD_AUDIO_DEVICE_END))
		return fclose(fd);

// AUDIO SETTINGS
	if(add_list_command(fd, (char *) CMD_AUDIO_SETTINGS))
		return fclose(fd);

	if(!copy_string(buffer, menuInSampleRate->value(), sizeof(buffer)))
		if(add_sub_command(fd, (char *)CMD_CAPTURE_SAMPLE_RATE, buffer, sizeof(buffer)))
			return fclose(fd);

	if(!copy_string(buffer, menuOutSampleRate->value(), sizeof(buffer)))
		if(add_sub_command(fd, (char *)CMD_PLAYBACK_SAMPLE_RATE, buffer, sizeof(buffer)))
			return fclose(fd);

	if(!copy_string(buffer, menuSampleConverter->value(), sizeof(buffer)))
		if(add_sub_command(fd, (char *)CMD_AUDIO_CONVERTER, buffer, sizeof(buffer)))
			return fclose(fd);

	if(!copy_int(buffer, (int) cntRxRateCorr->value(), sizeof(buffer)))
		if(add_sub_command(fd, (char *)CMD_RX_PPM, buffer, sizeof(buffer)))
			return fclose(fd);

	if(!copy_int(buffer, (int) cntTxRateCorr->value(), sizeof(buffer)))
		if(add_sub_command(fd, (char *)CMD_TX_PPM, buffer, sizeof(buffer)))
			return fclose(fd);

	if(!copy_int(buffer, (int) cntTxOffset->value(), sizeof(buffer)))
		if(add_sub_command(fd, (char *)CMD_TX_OFFSET, buffer, sizeof(buffer)))
			return fclose(fd);

	if(add_list_command(fd, (char *) CMD_AUDIO_SETTINGS_END))
		return fclose(fd);

// AUDIO RIGHT CHANNEL
	if(add_list_command(fd, (char *) CMD_AUDIO_RT_CHANNEL))
		return fclose(fd);

	if(!copy_bool(buffer, chkForceMono->value(), sizeof(buffer)))
		if(add_sub_command(fd, (char *)CMD_MONO_AUDIO, buffer, sizeof(buffer)))
			return fclose(fd);

	if(!copy_bool(buffer, chkAudioStereoOut->value(), sizeof(buffer)))
		if(add_sub_command(fd, (char *)CMD_AUDIO_L_R, buffer, sizeof(buffer)))
			return fclose(fd);

	if(!copy_bool(buffer, chkReverseAudio->value(), sizeof(buffer)))
		if(add_sub_command(fd, (char *)CMD_AUDIO_REV_L_R, buffer, sizeof(buffer)))
			return fclose(fd);

	if(!copy_bool(buffer, btnPTTrightchannel2->value(), sizeof(buffer)))
		if(add_sub_command(fd, (char *)CMD_PTT_RIGHT_CHAN, buffer, sizeof(buffer)))
			return fclose(fd);

	if(!copy_bool(buffer, btnQSK2->value(), sizeof(buffer)))
		if(add_sub_command(fd, (char *)CMD_CW_QSK_RT_CHAN, buffer, sizeof(buffer)))
			return fclose(fd);

	if(!copy_bool(buffer, chkPseudoFSK2->value(), sizeof(buffer)))
		if(add_sub_command(fd, (char *)CMD_PSEUDO_FSK_RT_CHAN, buffer, sizeof(buffer)))
			return fclose(fd);

	if(add_list_command(fd, (char *) CMD_AUDIO_RT_CHANNEL_END))
		return fclose(fd);

// AUDIO WAV
	if(!copy_string(buffer, listbox_wav_samplerate->value(), sizeof(buffer)))
		if(add_command(fd, (char *)CMD_WAVE_SAMPLE_RATE, buffer, sizeof(buffer)))
			return fclose(fd);

// RIG HRDWR PTT
	if(add_list_command(fd, (char *) CMD_HRDWR_PTT))
		return fclose(fd);

	if(!copy_bool(buffer, btnPTTrightchannel->value(), sizeof(buffer)))
		if(add_sub_command(fd, (char *)CMD_HPPT_PTT_RT, buffer, sizeof(buffer)))
			return fclose(fd);

	if(!copy_bool(buffer, btnTTYptt->value(), sizeof(buffer)))
		if(add_sub_command(fd, (char *)CMD_HPTT_SP2, buffer, sizeof(buffer)))
			return fclose(fd);

	if(!copy_string(buffer, inpTTYdev->value(), sizeof(buffer)))
		if(add_sub_command(fd, (char *)CMD_HPTT_SP2_PATH, buffer, sizeof(buffer)))
			return fclose(fd);

	if(!copy_bool(buffer, btnRTSptt->value(), sizeof(buffer)))
		if(add_sub_command(fd, (char *)CMD_HPTT_SP2_RTS, buffer, sizeof(buffer)))
			return fclose(fd);

	if(!copy_bool(buffer, btnRTSplusV->value(), sizeof(buffer)))
		if(add_sub_command(fd, (char *)CMD_HPTT_SP2_RTS_V, buffer, sizeof(buffer)))
			return fclose(fd);

	if(!copy_bool(buffer, btnDTRptt->value(), sizeof(buffer)))
		if(add_sub_command(fd, (char *)CMD_HPTT_SP2_DTR, buffer, sizeof(buffer)))
			return fclose(fd);

	if(!copy_bool(buffer, btnDTRplusV->value(), sizeof(buffer)))
		if(add_sub_command(fd, (char *)CMD_HPTT_SP2_DTR_V, buffer, sizeof(buffer)))
			return fclose(fd);

	if(!copy_bool(buffer, btnUsePPortPTT->value(), sizeof(buffer)))
		if(add_sub_command(fd, (char *)CMD_HPTT_PARALLEL, buffer, sizeof(buffer)))
			return fclose(fd);

	if(!copy_bool(buffer, btnUseUHrouterPTT->value(), sizeof(buffer)))
		if(add_sub_command(fd, (char *)CMD_HPTT_UHROUTER, buffer, sizeof(buffer)))
			return fclose(fd);

	if(add_list_command(fd, (char *) CMD_HRDWR_PTT_END))
		return fclose(fd);

// RIG CAT
	if(add_list_command(fd, (char *) CMD_RIGCAT))
		return fclose(fd);

	if(!copy_bool(buffer, chkUSERIGCAT->value(), sizeof(buffer)))
		if(add_sub_command(fd, (char *)CMD_RIGCAT_STATE, buffer, sizeof(buffer)))
			return fclose(fd);

	if(!copy_string(buffer, inpXmlRigDevice->value(), sizeof(buffer)))
		if(add_sub_command(fd, (char *)CMD_RIGCAT_DEV_PATH, buffer, sizeof(buffer)))
			return fclose(fd);

	if(!copy_string(buffer, progdefaults.XmlRigFilename.c_str(), sizeof(buffer)))
		if(add_sub_command(fd, (char *)CMD_RIGCAT_DESC_FILE, buffer, sizeof(buffer)))
			return fclose(fd);

	if(!copy_int(buffer, (int) cntRigCatRetries->value(), sizeof(buffer)))
		if(add_sub_command(fd, (char *)CMD_RIGCAT_RETRIES, buffer, sizeof(buffer)))
			return fclose(fd);

	if(!copy_int(buffer, (int) cntRigCatTimeout->value(), sizeof(buffer)))
		if(add_sub_command(fd, (char *)CMD_RIGCAT_RETRY_INTERVAL, buffer, sizeof(buffer)))
			return fclose(fd);

	if(!copy_int(buffer, (int) cntRigCatWait->value(), sizeof(buffer)))
		if(add_sub_command(fd, (char *)CMD_RIGCAT_WRITE_DELAY, buffer, sizeof(buffer)))
			return fclose(fd);

	if(!copy_int(buffer, (int) cntRigCatInitDelay->value(), sizeof(buffer)))
		if(add_sub_command(fd, (char *)CMD_RIGCAT_INTIAL_DELAY, buffer, sizeof(buffer)))
			return fclose(fd);

	if(!copy_string(buffer, listbox_xml_rig_baudrate->value(), sizeof(buffer)))
		if(add_sub_command(fd, (char *)CMD_RIGCAT_BAUD_RATE, buffer, sizeof(buffer)))
			return fclose(fd);

	if(!copy_int(buffer, (int) valRigCatStopbits->value(), sizeof(buffer)))
		if(add_sub_command(fd, (char *)CMD_RIGCAT_STOP_BITS, buffer, sizeof(buffer)))
			return fclose(fd);

	if(!copy_bool(buffer, btnRigCatEcho->value(), sizeof(buffer)))
		if(add_sub_command(fd, (char *)CMD_RIGCAT_ECHO, buffer, sizeof(buffer)))
			return fclose(fd);

	if(!copy_bool(buffer, btnRigCatRTSptt->value(), sizeof(buffer)))
		if(add_sub_command(fd, (char *)CMD_RIGCAT_TOGGLE_RTS_PTT, buffer, sizeof(buffer)))
			return fclose(fd);

	if(!copy_bool(buffer, chk_restore_tio->value(), sizeof(buffer)))
		if(add_sub_command(fd, (char *)CMD_RIGCAT_RESTORE, buffer, sizeof(buffer)))
			return fclose(fd);

	if(!copy_bool(buffer, btnRigCatCMDptt->value(), sizeof(buffer)))
		if(add_sub_command(fd, (char *)CMD_RIGCAT_PTT_COMMAND, buffer, sizeof(buffer)))
			return fclose(fd);

	if(!copy_bool(buffer, btnRigCatDTRptt->value(), sizeof(buffer)))
		if(add_sub_command(fd, (char *)CMD_RIGCAT_TOGGLE_DTR_PTT, buffer, sizeof(buffer)))
			return fclose(fd);

	if(!copy_bool(buffer, btnRigCatRTSptt->value(), sizeof(buffer)))
		if(add_sub_command(fd, (char *)CMD_RIGCAT_TOGGLE_RTS_PTT, buffer, sizeof(buffer)))
			return fclose(fd);

	if(!copy_bool(buffer, btnRigCatRTSplus->value(), sizeof(buffer)))
		if(add_sub_command(fd, (char *)CMD_RIGCAT_RTS_12V, buffer, sizeof(buffer)))
			return fclose(fd);

	if(!copy_bool(buffer, btnRigCatDTRplus->value(), sizeof(buffer)))
		if(add_sub_command(fd, (char *)CMD_RIGCAT_DTR_12V, buffer, sizeof(buffer)))
			return fclose(fd);

	if(!copy_bool(buffer, chkRigCatRTSCTSflow->value(), sizeof(buffer)))
		if(add_sub_command(fd, (char *)CMD_RIGCAT_HRDWR_FLOW, buffer, sizeof(buffer)))
			return fclose(fd);

	if(!copy_bool(buffer, chkRigCatVSP->value(), sizeof(buffer)))
		if(add_sub_command(fd, (char *)CMD_RIGCAT_VSP, buffer, sizeof(buffer)))
			return fclose(fd);

	if(add_list_command(fd, (char *) CMD_RIGCAT_END))
		return fclose(fd);

// HAMLIB
	if(add_list_command(fd, (char *) CMD_HAMLIB))
		return fclose(fd);

	if(!copy_bool(buffer, chkUSEHAMLIB->value(), sizeof(buffer)))
		if(add_sub_command(fd, (char *)CMD_HAMLIB_STATE, buffer, sizeof(buffer)))
			return fclose(fd);

	if(!copy_string(buffer, cboHamlibRig->value(), sizeof(buffer)))
		if(add_sub_command(fd, (char *)CMD_HAMLIB_RIG, buffer, sizeof(buffer)))
			return fclose(fd);

	if(!copy_string(buffer, inpRIGdev->value(), sizeof(buffer)))
		if(add_sub_command(fd, (char *)CMD_HAMLIB_DEV_PATH, buffer, sizeof(buffer)))
			return fclose(fd);

	if(!copy_int(buffer, (int) cntHamlibRetries->value(), sizeof(buffer)))
		if(add_sub_command(fd, (char *)CMD_HAMLIB_RETRIES, buffer, sizeof(buffer)))
			return fclose(fd);

	if(!copy_int(buffer, (int) cntHamlibTimeout->value(), sizeof(buffer)))
		if(add_sub_command(fd, (char *)CMD_HAMLIB_RETRY_INTERVAL, buffer, sizeof(buffer)))
			return fclose(fd);

	if(!copy_int(buffer, (int) cntHamlibWriteDelay->value(), sizeof(buffer)))
		if(add_sub_command(fd, (char *)CMD_HAMLIB_WRITE_DELAY, buffer, sizeof(buffer)))
			return fclose(fd);

	if(!copy_int(buffer, (int) cntHamlibWait->value(), sizeof(buffer)))
		if(add_sub_command(fd, (char *)CMD_HAMLIB_POST_WRITE_DELAY, buffer, sizeof(buffer)))
			return fclose(fd);

	if(!copy_string(buffer, listbox_baudrate->value(), sizeof(buffer)))
		if(add_sub_command(fd, (char *)CMD_HAMLIB_BAUD_RATE, buffer, sizeof(buffer)))
			return fclose(fd);

	if(!copy_int(buffer, (int) valHamRigStopbits->value(), sizeof(buffer)))
		if(add_sub_command(fd, (char *)CMD_HAMLIB_STOP_BITS, buffer, sizeof(buffer)))
			return fclose(fd);

	if(!copy_string(buffer, listbox_sideband->value(), sizeof(buffer)))
		if(add_sub_command(fd, (char *)CMD_HAMLIB_SIDE_BAND, buffer, sizeof(buffer)))
			return fclose(fd);

	if(!copy_bool(buffer, btnHamlibCMDptt->value(), sizeof(buffer)))
		if(add_sub_command(fd, (char *)CMD_HAMLIB_PTT_COMMAND, buffer, sizeof(buffer)))
			return fclose(fd);

	if(!copy_bool(buffer, btnHamlibDTRplus->value(), sizeof(buffer)))
		if(add_sub_command(fd, (char *)CMD_HAMLIB_DTR_12V, buffer, sizeof(buffer)))
			return fclose(fd);

	if(!copy_bool(buffer, btnHamlibDTRplus->value(), sizeof(buffer)))
		if(add_sub_command(fd, (char *)CMD_HAMLIB_RTS_12V, buffer, sizeof(buffer)))
			return fclose(fd);

	if(!copy_bool(buffer, chkHamlibRTSCTSflow->value(), sizeof(buffer)))
		if(add_sub_command(fd, (char *)CMD_HAMLIB_HRDWR_FLOW, buffer, sizeof(buffer)))
			return fclose(fd);

	if(!copy_bool(buffer, chkHamlibXONXOFFflow->value(), sizeof(buffer)))
		if(add_sub_command(fd, (char *)CMD_HAMLIB_SFTWR_FLOW, buffer, sizeof(buffer)))
			return fclose(fd);

	if(!copy_string(buffer, inpHamlibConfig->value(), sizeof(buffer)))
		if(add_sub_command(fd, (char *)CMD_HAMLIB_ADV_CONFIG, buffer, sizeof(buffer)))
			return fclose(fd);

	if(add_list_command(fd, (char *) CMD_HAMLIB_END))
		return fclose(fd);

// XMLRPC RC
	if(add_list_command(fd, (char *) CMD_RC_XMLRPC))
		return fclose(fd);

	if(!copy_bool(buffer, chkHamlibXONXOFFflow->value(), sizeof(buffer)))
		if(add_sub_command(fd, (char *)CMD_RC_XMLRPC_STATE, buffer, sizeof(buffer)))
			return fclose(fd);

	if(!copy_double(buffer, mbw_delay->value(), sizeof(buffer)))
		if(add_sub_command(fd, (char *)CMD_XMLRPC_BW_DELAY, buffer, sizeof(buffer)))
			return fclose(fd);

	if(add_list_command(fd, (char *) CMD_RC_XMLRPC_END))
		return fclose(fd);


	fclose(fd);
}

/** ********************************************************
 * \brief Add top level list command to script file
 * \param fd File descriptor
 * \param cmd Pointer to the file name and path.
 * \return 0 OK, other Error
 ***********************************************************/
static int add_list_command(FILE *fd, char *cmd)
{
	if(!fd || !cmd)
		return -1;

	fprintf(fd, "%s:\n", cmd);

	return ferror(fd);
}

/** ********************************************************
 * \brief Add command and paramter to script file
 * \param fd File descriptor
 * \param cmd Pointer to the file name and path.
 * \return 0 OK, other Error
 ***********************************************************/
static int add_command(FILE *fd, char *cmd, char *param, size_t size)
{
	if(!fd || !cmd || !param)
		return -1;

	bool q_flag = false;

	for(int i = 0; i < size; i++) {
		if(param[i] == 0) break;
		if((param[i] == ',') || (param[i] <= ' ')) {
			q_flag = true;
		}

		if(param[i] == '"') param[i] = '\'';
	}

	if(q_flag)
		fprintf(fd, "%s:\"%s\"\n", cmd, param);
	else
		fprintf(fd, "%s:%s\n", cmd, param);

	return ferror(fd);
}

/** ********************************************************
 * \brief Add command and paramter to script file
 * \param fd File descriptor
 * \param cmd Pointer to the file name and path.
 * \return 0 OK, other Error
 ***********************************************************/
static int add_sub_command(FILE *fd, char *cmd, char *param, size_t size)
{
	if(!fd || !cmd || !param)
		return -1;

	bool q_flag = false;

	for(int i = 0; i < size; i++) {
		if(param[i] == 0) break;
		if((param[i] == ',') || (param[i] <= ' ')) {
			q_flag = true;
		}

		if(param[i] == '"') param[i] = '\'';
	}

	if(q_flag)
		fprintf(fd, "   %s:\"%s\"\n", cmd, param);
	else
		fprintf(fd, "   %s:%s\n", cmd, param);

	return ferror(fd);
}

/** ********************************************************
 * \brief Copy a series of charaters.
 * \param dst char pointer to store the data.
 * \param src char pointer tot he source data.
 * \param size size_t of destination buffer.
 * \return 0 OK, other Error
 ***********************************************************/
static int copy_string(char *dst, const char *src, size_t size)
{
	if(dst && src && size) {
		if(str_n_len(src, size)) {
			memset(dst, 0, size);
			strncpy(dst, src, size);
			return 0;
		}
	}

	return -1;
}

/** ********************************************************
 * \brief Convert bool to string
 * \param dst char pointer to store the data.
 * \param src bool value.
 * \param size size_t of destination buffer.
 * \return 0 OK, other Error
 ***********************************************************/
static int copy_bool(char *dst, bool src, size_t size)
{
	if(!dst || size < 1) {
		return -1;
	}

	memset(dst, 0, size);

	if(src)
		strncpy(dst, (char *)"ENABLE", size);
	else
		strncpy(dst, (char *)"DISABLE", size);

	return 0;
}

/** ********************************************************
 * \brief Convert integer to string
 * \param dst char pointer to store the data.
 * \param src integer value.
 * \param size size_t of destination buffer.
 * \return 0 OK, other Error
 ***********************************************************/
static int copy_int(char *dst, int src, size_t size)
{
	if(!dst || size < 1) {
		return -1;
	}

	memset(dst, 0, size);
	snprintf(dst, size - 1, "%d", src);

	return 0;
}
/** ********************************************************
 * \brief Convert floating point to string
 * \param dst char pointer to store the data.
 * \param src integer value.
 * \param size size_t of destination buffer.
 * \return 0 OK, other Error
 ***********************************************************/
static int copy_double(char *dst, double src, size_t size)
{
	if(!dst || size < 1) {
		return -1;
	}

	memset(dst, 0, size);
	snprintf(dst, size - 1, "%f", src);

	return 0;
}

/** ********************************************************
 * \brief Count string length with imposed limit
 * \param src char pointer.
 * \param size size_t of source character buffer.
 * \return 0 OK, other Error
 ***********************************************************/
static int str_n_len(const char *src, size_t size)
{
	if(!src || size < 1)
		return 0;

	int i = 0;

	for(i = 0; i < size; i++)
		if(src[i] == 0) break;

	return i;
}
