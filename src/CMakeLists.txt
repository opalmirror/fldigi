# Copyright (C) 2007-2009 Stelios Bounanos, M0GLD (m0gld AT enotty DOT net)
# Copyright (c) 2008 Dave Freese, W1HKJ (w1hkj AT w1hkj DOT com)
# Copyright (c) 2012 Remi Chateauneu, F4ECW (remi dot chateauneu AT gmail DOT com)
# License: GPLv3+: GNU GPL version 3 or later.

# Sources that we build. It is OK to have headers here.
set(fldigi_SOURCES
	combo/combo.cxx
	cw_rtty/cw.cxx
	cw_rtty/morse.cxx
	cw_rtty/rtty.cxx
	cw_rtty/view_rtty.cxx
	contestia/contestia.cxx
	#dialogs/confdialog.cxx
	dialogs/fl_digi.cxx
	dialogs/font_browser.cxx
	dialogs/Viewer.cxx
	dialogs/htmlstrings.cxx
	#dialogs/notifydialog.cxx
	#dialogs/record_browse.cxx
	dtmf/dtmf.cxx
	thor/thor.cxx
	thor/thorvaricode.cxx
	dominoex/dominoex.cxx
	dominoex/dominovar.cxx
	feld/feld.cxx
	feld/feldfonts.cxx
	fileselector/FL/Native_File_Chooser.H
	fileselector/Native_File_Chooser.cxx
	fileselector/fileselect.cxx
	filters/fftfilt.cxx
	filters/filters.cxx
	filters/viterbi.cxx
	globals/globals.cxx
	include/htmlstrings.h
	include/arq_io.h
	include/confdialog.h
	include/dtmf.h
	include/FTextView.h
	include/FTextRXTX.h
	include/fileselect.h
	include/Panel.h
	include/FreqControl.h
	include/analysis.h
	include/ascii.h
	include/charsetdistiller.h
	include/charsetlist.h
	include/colorbox.h
	include/combo.h
	include/complex.h
	include/configuration.h
	include/contestia.h
	include/cw.h
	include/debug.h
	include/digiscope.h
	include/dxcc.h
	include/thor.h
	include/thorvaricode.h
	include/dominoex.h
	include/dominovar.h
	include/feld.h
	include/fftfilt.h
	include/filters.h
	include/fl_digi.h
	include/fl_lock.h
	include/fldigi-config.h
	include/flinput2.h
	include/flmisc.h
	include/flslider2.h
	include/font_browser.h
	include/fontdef.h
	include/gettext.h
	include/globals.h
	include/icons.h
	include/interleave.h
	include/jalocha/pj_cmpx.h
	include/jalocha/pj_fft.h
	include/jalocha/pj_fht.h
	include/jalocha/pj_fifo.h
	include/jalocha/pj_gray.h
	include/jalocha/pj_lowpass3.h
	include/jalocha/pj_mfsk.h
	include/jalocha/pj_struc.h
	include/coordinate.h
	include/gfft.h
	include/kmlserver.h
	include/locator.h
	include/log.h
	include/logger.h
	include/macroedit.h
	include/macros.h
	include/main.h
	include/mbuffer.h
	include/mfsk.h
	include/mfskvaricode.h
	include/wefax.h
	include/wefax-pic.h
	include/navtex.h
	include/misc.h
	include/mixer.h
	include/modem.h
	include/morse.h
	include/mt63base.h
	include/mt63.h
	include/network.h
	include/dsp.h
	include/newinstall.h
	include/notify.h
	include/notifydialog.h
	include/record_browse.h
	include/olivia.h
	include/pkg.h
	include/picture.h
	include/progress.h
	include/psk.h
	include/viewpsk.h
	include/pskcoeff.h
	include/pskvaricode.h
	include/pskeval.h
	include/ptt.h
	include/pixmaps.h
	include/pskrep.h
	include/qrunner.h
	include/lookupcall.h
	include/qrzlib.h
	include/raster.h
	include/re.h
	include/rigCAT.h
	include/rigMEM.h
	include/rigio.h
	include/rigsupport.h
	include/rigxml.h
	include/ringbuffer.h
	include/rsid.h
	include/rtty.h
	include/view_rtty.h
	include/pkt.h
	include/nco.h
	include/synop.h
	include/nullmodem.h
	include/record_loader.h
	include/record_loader_gui.h
	include/rx_extract.h
	include/speak.h
	include/serial.h
	include/socket.h
	include/sound.h
	include/soundconf.h
	include/spot.h
	include/ssb.h
	include/stacktrace.h
	include/status.h
	include/strutil.h
	include/testmodem.h
	include/threads.h
	include/throb.h
	include/timeops.h
	include/trx.h
	include/util.h
	include/Viewer.h
	include/viterbi.h
	include/waterfall.h
	include/wwv.h
	include/xmlreader.h
	include/adif_def.h
	include/adif_io.h
	include/calendar.h
	include/date.h
	include/field_def.h
	include/logbook.h
	include/lgbook.h
	include/logsupport.h
	include/outputencoder.h
	include/qso_db.h
	include/table.h
	include/textio.h
	include/psk_browser.h
	include/utf8file_io.h
	include/weather.h
	irrxml/CXMLReaderImpl.h
	irrxml/fast_atof.h
	irrxml/heapsort.h
	irrxml/irrArray.h
	irrxml/irrString.h
	irrxml/irrTypes.h
	irrxml/irrXML.cpp
	irrxml/irrXML.h
	logbook/adif_io.cxx
	logbook/calendar.cxx
	logbook/date.cxx
	#logbook/lgbook.cxx
	logbook/logbook.cxx
	logbook/logsupport.cxx
	logbook/lookupcall.cxx
	logbook/qrzlib.cxx
	logbook/qso_db.cxx
	logbook/table.cxx
	logbook/textio.cxx
	logger/logger.cxx
	logger/rx_extract.cxx
	logger/speak.cxx
	main.cxx
	mfsk/interleave.cxx
	mfsk/mfsk.cxx
	mfsk/mfskvaricode.cxx
	wefax/wefax.cxx
	wefax/wefax-pic.cxx
	navtex/navtex.cxx
	misc/ascii.cxx
	misc/charsetdistiller.cxx
	misc/charsetlist.cxx
	misc/configuration.cxx
	misc/coordinate.cxx
	misc/debug.cxx
	misc/dxcc.cxx
	misc/icons.cxx
	misc/kmlserver.cxx
	misc/log.cxx
	misc/macroedit.cxx
	misc/macros.cxx
	misc/misc.cxx
	misc/network.cxx
	misc/newinstall.cxx
	misc/outputencoder.cxx
	misc/arq_io.cxx
	misc/pixmaps.cxx
	misc/pixmaps_tango.cxx
	misc/re.cxx
	misc/record_loader.cxx
	misc/socket.cxx
	misc/stacktrace.cxx
	misc/status.cxx
	misc/strutil.cxx
	misc/threads.cxx
	misc/timeops.cxx
	misc/utf8file_io.cxx
	misc/util.cxx
	misc/weather.cxx
	mt63/dsp.cxx
	mt63/mt63.cxx
	mt63/mt63base.cxx
	include/navtex.h
	olivia/olivia.cxx
	psk/psk.cxx
	psk/pskcoeff.cxx
	psk/pskvaricode.cxx
	psk/viewpsk.cxx
	psk/pskeval.cxx
	qrunner/fqueue.h
	qrunner/qrunner.cxx
	rigcontrol/FreqControl.cxx
	rigcontrol/ptt.cxx
	rigcontrol/rigMEM.cxx
	rigcontrol/rigio.cxx
	rigcontrol/rigsupport.cxx
	rigcontrol/rigxml.cxx
	rigcontrol/serial.cxx
	rsid/rsid.cxx
	soundcard/mixer.cxx
	soundcard/sound.cxx
	soundcard/soundconf.cxx
	spot/notify.cxx
	spot/pskrep.cxx
	spot/spot.cxx
	ssb/ssb.cxx
	synop-src/synop.cxx
	throb/throb.cxx
	packet/pkt.cxx
	trx/modem.cxx
	trx/nullmodem.cxx
	trx/trx.cxx
	waterfall/colorbox.cxx
	waterfall/digiscope.cxx
	waterfall/raster.cxx
	waterfall/waterfall.cxx
	widgets/Fl_Text_Buffer_mod.cxx
	widgets/Fl_Text_Display_mod.cxx
	widgets/Fl_Text_Editor_mod.cxx
	widgets/FTextView.cxx
	widgets/FTextRXTX.cxx
	widgets/Panel.cxx
	widgets/flinput2.cxx
	widgets/flmisc.cxx
	widgets/flslider2.cxx
	widgets/picture.cxx
	widgets/progress.cxx
	widgets/psk_browser.cxx
	wwv/analysis.cxx
	wwv/wwv.cxx
	logbook/xmlrpc_log.cxx
	xmlrpcpp/XmlRpc.h
	xmlrpcpp/XmlRpcClient.cpp
	xmlrpcpp/XmlRpcClient.h
	xmlrpcpp/XmlRpcDispatch.cpp
	xmlrpcpp/XmlRpcDispatch.h
	xmlrpcpp/XmlRpcException.h
	xmlrpcpp/XmlRpcMutex.cpp
	xmlrpcpp/XmlRpcMutex.h
	xmlrpcpp/XmlRpcServer.cpp
	xmlrpcpp/XmlRpcServer.h
	xmlrpcpp/XmlRpcServerConnection.cpp
	xmlrpcpp/XmlRpcServerConnection.h
	xmlrpcpp/XmlRpcServerMethod.cpp
	xmlrpcpp/XmlRpcServerMethod.h
	xmlrpcpp/XmlRpcSocket.cpp
	xmlrpcpp/XmlRpcSocket.h
	xmlrpcpp/XmlRpcSource.cpp
	xmlrpcpp/XmlRpcSource.h
	xmlrpcpp/XmlRpcThread.cpp
	xmlrpcpp/XmlRpcThread.h
	xmlrpcpp/XmlRpcThreadedServer.cpp
	xmlrpcpp/XmlRpcThreadedServer.h
	xmlrpcpp/XmlRpcUtil.cpp
	xmlrpcpp/XmlRpcUtil.h
	xmlrpcpp/XmlRpcValue.cpp
	xmlrpcpp/XmlRpcValue.h
	xmlrpcpp/base64.h
	libtiniconv/tiniconv.c
	libtiniconv/tiniconv_desc.c
        include/xmlrpc.h misc/xmlrpc.cxx
)

# Sources that are part of the distribution but are not compiled directly
set(EXTRA_fldigi_SOURCES
	blank/blank.cxx
	blank/blank.h
	fileselector/flnfc_common.cxx
	feld/Feld7x7-14.cxx
	feld/Feld7x7n-14.cxx
	feld/FeldDx-14.cxx
	feld/FeldFat-14.cxx
	feld/FeldHell-12.cxx
	feld/FeldLittle-12.cxx
	feld/FeldLo8-14.cxx
	feld/FeldLow-14.cxx
	feld/FeldModern-14.cxx
	feld/FeldModern8-14.cxx
	feld/FeldNarr-14.cxx
	feld/FeldReal-14.cxx
	feld/FeldStyl-14.cxx
	feld/FeldVert-14.cxx
	feld/FeldWide-14.cxx
	include/Fl_Text_Buffer_mod.H
	include/Fl_Text_Display_mod.H
	include/Fl_Text_Editor_mod.H
	mfsk/mfsk-pic.cxx
	mt63/alias_1k.dat
	mt63/alias_2k.dat
	mt63/morse.dat
	mt63/symbol.dat
	mt63/alias_k5.dat
	mt63/mt63intl.dat
	rsid/rsid_defs.cxx
	trx/tune.cxx
	dialogs/guide.cxx
	libtiniconv/tiniconv.h
	libtiniconv/tiniconv_int.h
	libtiniconv/encdec/ascii.h
	libtiniconv/encdec/big5.h
	libtiniconv/encdec/ces_big5.h
	libtiniconv/encdec/ces_gbk.h
	libtiniconv/encdec/cp1250.h
	libtiniconv/encdec/cp1251.h
	libtiniconv/encdec/cp1252.h
	libtiniconv/encdec/cp1253.h
	libtiniconv/encdec/cp1254.h
	libtiniconv/encdec/cp1255.h
	libtiniconv/encdec/cp1256.h
	libtiniconv/encdec/cp1257.h
	libtiniconv/encdec/cp1258.h
	libtiniconv/encdec/cp866.h
	libtiniconv/encdec/cp936ext.h
	libtiniconv/encdec/cp936.h
	libtiniconv/encdec/euc_cn.h
	libtiniconv/encdec/flushwc.h
	libtiniconv/encdec/gb2312.h
	libtiniconv/encdec/gbkext1.h
	libtiniconv/encdec/gbkext2.h
	libtiniconv/encdec/gbkext_inv.h
	libtiniconv/encdec/gbk.h
	libtiniconv/encdec/iso2022_jp.h
	libtiniconv/encdec/iso8859_10.h
	libtiniconv/encdec/iso8859_11.h
	libtiniconv/encdec/iso8859_13.h
	libtiniconv/encdec/iso8859_14.h
	libtiniconv/encdec/iso8859_15.h
	libtiniconv/encdec/iso8859_16.h
	libtiniconv/encdec/iso8859_1.h
	libtiniconv/encdec/iso8859_2.h
	libtiniconv/encdec/iso8859_3.h
	libtiniconv/encdec/iso8859_4.h
	libtiniconv/encdec/iso8859_5.h
	libtiniconv/encdec/iso8859_6.h
	libtiniconv/encdec/iso8859_7.h
	libtiniconv/encdec/iso8859_8.h
	libtiniconv/encdec/iso8859_9.h
	libtiniconv/encdec/jisx0201.h
	libtiniconv/encdec/jisx0208.h
	libtiniconv/encdec/koi8_r.h
	libtiniconv/encdec/koi8_ru.h
	libtiniconv/encdec/koi8_u.h
	libtiniconv/encdec/mac_cyrillic.h
	libtiniconv/encdec/ucs2.h
	libtiniconv/encdec/utf7.h
	libtiniconv/encdec/utf8.h
	libtiniconv/encdec/vietcomb.h
)

set(flarq_SOURCES
	dialogs/font_browser.cxx
	flarq-src/arq.cxx
	#flarq-src/arqdialogs.cxx
	flarq-src/arqhelp.cxx
	flarq-src/b64.cxx
	flarq-src/flarq.cxx
	flarq-src/flarqenv.cxx
	flarq-src/include/arq.h
	flarq-src/include/arqdialogs.h
	flarq-src/include/b64.h
	flarq-src/include/flarq.h
	flarq-src/include/flarqenv.h
	include/Fl_Text_Display_mod.H
	include/Fl_Text_Editor_mod.H
	include/FTextView.h
	include/debug.h
	include/fileselect.h
	include/flinput2.h
	include/flmisc.h
	include/flslider2.h
	include/font_browser.h
	include/icons.h
	include/pixmaps.h
	include/re.h
	include/socket.h
	include/stacktrace.h
	include/threads.h
	include/table.h
	include/util.h
	combo/combo.cxx
	fileselector/FL/Native_File_Chooser.H
	fileselector/Native_File_Chooser.cxx
	fileselector/fileselect.cxx
	logbook/table.cxx
	misc/ascii.cxx
	misc/debug.cxx
	misc/stacktrace.cxx
	misc/threads.cxx
	misc/timeops.cxx
	widgets/flinput2.cxx
	widgets/flmisc.cxx
	widgets/flslider2.cxx
	misc/icons.cxx
	misc/pixmaps.cxx
	misc/pixmaps_tango.cxx
	misc/re.cxx
	misc/socket.cxx
	misc/util.cxx
	widgets/Fl_Text_Buffer_mod.cxx
	widgets/Fl_Text_Display_mod.cxx
	widgets/Fl_Text_Editor_mod.cxx
	widgets/FTextView.cxx
)

set(HAMLIB_SRC include/hamlib.h rigcontrol/hamlib.cxx include/rigclass.h rigcontrol/rigclass.cxx)
set(XMLRPC_SRC include/xmlrpc.h misc/xmlrpc.cxx)
set(FLDIGI_WIN32_RES_SRC fldigirc.rc)
set(FLARQ_WIN32_RES_SRC flarq-src/flarqrc.rc)
set(COMMON_WIN32_RES_SRC common.rc)
set(LOCATOR_SRC misc/locator.c)
set(BENCHMARK_SRC include/benchmark.h misc/benchmark.cxx)
set(REGEX_SRC compat/regex.h compat/regex.c)
set(STACK_SRC include/stack.h misc/stack.cxx)
set(MINGW32_SRC include/compat.h compat/getsysinfo.c compat/mingw.c compat/mingw.h)
set(NLS_SRC misc/nls.cxx include/nls.h)

if(ENABLE_HAMLIB)
    list(APPEND fldigi_SOURCES ${HAMLIB_SRC})
endif()

if(NEED_HAMLIB_LOCATOR)
    list(APPEND fldigi_SOURCES ${LOCATOR_SRC})
endif()

if(ENABLE_BENCHMARK)
    list(APPEND fldigi_SOURCES ${BENCHMARK_SRC})
endif()

if(COMPAT_REGEX)
    list(APPEND fldigi_SOURCES ${REGEX_SRC})
    list(APPEND flarq_SOURCES ${REGEX_SRC})
endif()

if(COMPAT_STACK)
    list(APPEND fldigi_SOURCES ${STACK_SRC})
    list(APPEND flarq_SOURCES ${STACK_SRC})
endif()

if(MINGW)
    list(APPEND fldigi_SOURCES ${MINGW32_SRC})
    list(APPEND flarq_SOURCES ${MINGW32_SRC})
endif()

if(ENABLE_NLS)
    list(APPEND fldigi_SOURCES ${NLS_SRC})
    list(APPEND flarq_SOURCES ${NLS_SRC})
endif()

########################################################################


#	$(call silent,WRES  ,$@)$(WINDRES) -DRC_BUILD_TIME=\\\"$(shell date +%s)\\\" -I${CMAKE_CURRENT_SOURCE_DIR} -Iinclude -I../data/win32 $< -O coff $@
#fldigi_SOURCES += $(FLDIGI_WIN32_RES_SRC)
#flarq_SOURCES += $(FLARQ_WIN32_RES_SRC)
if(WIN32 AND HAVE_WINDRES)
    list(APPEND fldigi_SOURCES ${FLDIGI_WIN32_RES_SRC})
    list(APPEND flarq_SOURCES ${FLARQ_WIN32_RES_SRC})
endif()

#install-exec-local:
#if WANT_FLDIGI
#	if test -f ../scripts/fldigi-shell; then
#	    $(mkinstalldirs) $(DESTDIR)/$(bindir);
#            $(INSTALL_SCRIPT) ../scripts/fldigi-shell $(DESTDIR)/$(bindir);
#        fi
#	if test -f ../scripts/ftp_kml_files.sh; then
#	    $(mkinstalldirs) $(DESTDIR)/$(bindir);
#        $(INSTALL_SCRIPT) ../scripts/ftp_kml_files.sh $(DESTDIR)/$(bindir);
#	fi
#endif

#	if test -f ../data/NAVTEX_Stations.csv; then
#	    $(mkinstalldirs) $(DESTDIR)/$(datadir)/fldigi;
#	    $(INSTALL_DATA) ../data/NAVTEX_Stations.csv $(DESTDIR)/$(pkgdatadir);
#	fi
#	if test -f ../data/nsd_bbsss.txt; then
#	    $(mkinstalldirs) $(DESTDIR)/$(datadir)/fldigi;
#	    $(INSTALL_DATA) ../data/nsd_bbsss.txt $(DESTDIR)/$(pkgdatadir);
#	fi
#	if test -f ../data/station_table.txt; then
#	    $(mkinstalldirs) $(DESTDIR)/$(datadir)/fldigi;
#	    $(INSTALL_DATA) ../data/station_table.txt $(DESTDIR)/$(pkgdatadir);
#	fi
#	if test -f ../data/ToR-Stats-SHIP.csv; then
#	    $(mkinstalldirs) $(DESTDIR)/$(datadir)/fldigi;
#	    $(INSTALL_DATA) ../data/ToR-Stats-SHIP.csv $(DESTDIR)/$(pkgdatadir);
#	fi
#	if test -f ../data/wmo_list.txt; then
#	    $(mkinstalldirs) $(DESTDIR)/$(datadir)/fldigi;
#	    $(INSTALL_DATA) ../data/wmo_list.txt $(DESTDIR)/$(pkgdatadir);
#	fi
#	if test -f ../kml/styles.kml; then
#	    $(mkinstalldirs) $(DESTDIR)/$(pkgdatadir)/kml;
#	    $(INSTALL_DATA) ../kml/styles.kml $(DESTDIR)/$(pkgdatadir)/kml;
#	fi
#endif

set(FLDIGI_FL_SRC
	dialogs/confdialog.fl
	dialogs/notifydialog.fl
	dialogs/record_browse.fl
	logbook/lgbook.fl
)

set(FLARQ_FL_SRC flarq-src/arqdialogs.fl)

if(HAVE_FLUID AND WANT_FLDIGI)
    fltk_wrap_ui(fldigi_ui ${FLDIGI_FL_SRC})
    add_library(fldigi_ui ${fldigi_ui_FLTK_UI_SRCS})
    set_target_properties(fldigi_ui PROPERTIES
        COMPILE_DEFINITIONS "BUILD_FLDIGI")
endif()

if(HAVE_FLUID AND WANT_FLARQ)
    fltk_wrap_ui(flarq_ui ${FLARQ_FL_SRC})
    add_library(flarq_ui ${flarq_ui_FLTK_UI_SRCS})
    target_include_directories(flarq_ui PRIVATE flarq-src/include)
endif()

#if WANT_FLDIGI
#    WANT_FLDIGI = yes
#    INSTALLER_FILE = $(PACKAGE_TARNAME)-$(PACKAGE_VERSION)_setup.exe
#    APPBUNDLE=$(PACKAGE_TARNAME)-$(PACKAGE_VERSION)
#    APPBUNDLE_NOLIBS=$(APPBUNDLE)-nolibs
#endif
#if WANT_FLARQ
#    WANT_FLARQ = yes
#if !WANT_FLDIGI
#        INSTALLER_FILE = flarq-$(FLARQ_VERSION)_setup.exe
#        APPBUNDLE=flarq-$(FLARQ_VERSION)
#        APPBUNDLE_NOLIBS=$(APPBUNDLE)-nolibs
#endif
#endif

#if DARWIN
#appbundle: $(bin_PROGRAMS)
#	$(call silent,APPB  ,$(APPBUNDLE_NOLIBS) $(APPBUNDLE))sh ../scripts/mkappbundle.sh "../data" .
#    CLEAN_LOCAL += $(APPBUNDLE_NOLIBS) $(APPBUNDLE) $(APPBUNDLE)*.dmg
#endif

#if HAVE_NSIS
#nsisinst: $(bin_PROGRAMS)
#	$(call silent,NSIS  ,$(INSTALLER_FILE))sh ../scripts/mknsisinst.sh "../data" .
#    CLEANFILES += $(INSTALLER_FILE)
#endif

#if WANT_FLDIGI
#if ENABLE_HAMLIB
#hamlib-static: $(fldigi_OBJECTS)
#	$(call silent,HLS   ,fldigi$(EXEEXT))sh ../scripts/mkhamlibstatic.sh fldigi
#endif
#endif

#tmp_srcdir_var=${CMAKE_CURRENT_SOURCE_DIR}
#TESTS = $(tmp_srcdir_var)/../scripts/tests/config-h.sh $(tmp_srcdir_var)/../scripts/tests/cr.sh

#$(builddir)/dialogs/guide.cxx: $(builddir)/../doc/guide.html
#	@mkdir -p $(builddir)/dialogs
#	$(call silent,GUIDE ,$@)sed 's/\\/\\\\/g; s/"/\\"/g; s/$$/\\n\\/g; 1 s/.*/const char* szBeginner = "&/; $$ s/.*/&n";/' $< > $@
#dialogs/htmlstrings.cxx: $(builddir)/dialogs/guide.cxx


if(WANT_FLDIGI)
    add_executable(fldigi ${fldigi_SOURCES})
    add_dependencies(fldigi fldigi_ui)
    target_link_libraries(fldigi ${FLDIGI_LINK_LIBS} ${CMAKE_DL_LIBS} fldigi_ui)
    set_target_properties (fldigi PROPERTIES
        COMPILE_DEFINITIONS "BUILD_FLDIGI -DPKGDATADIR=\"${CMAKE_INSTALL_FULL_DATADIR}\"")
    install(TARGETS fldigi
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
    install(PROGRAMS ../scripts/fldigi-shell
        DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()

if(WANT_FLARQ)
    add_executable(flarq ${flarq_SOURCES})
    add_dependencies(flarq flarq_ui)
    target_link_libraries(flarq ${FLARQ_LINK_LIBS} ${CMAKE_DL_LIBS} flarq_ui)
    target_include_directories(flarq PRIVATE flarq-src/include)
    set_target_properties (flarq PROPERTIES COMPILE_DEFINITIONS "BUILD_FLARQ")
    install(TARGETS flarq
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()
