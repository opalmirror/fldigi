set(FLDIGI_DATA
    #fldigi-psk.png
    #fldigi-rtty.png
    NAVTEX_Stations.csv
    nsd_bbsss.txt
    station_table.txt
    ToR-Stats-SHIP.csv
    #mac/Info.plist.in
    #mac/fldigi.icns
    #win32/fldigi.ico
    #mac/flarq.icns
    #win32/flarq.ico
)

set(FLDIGI_ICONS
    fldigi.xpm
    flarq.xpm
)

set(FLDIGI_DESKTOP
    fldigi.desktop
    flarq.desktop
)

install(FILES ${FLDIGI_DATA}
    DESTINATION ${CMAKE_INSTALL_DATADIR}/fldigi
)

install(FILES ${FLDIGI_ICONS}
    DESTINATION ${CMAKE_INSTALL_DATADIR}/pixmaps
)

install(FILES ${FLDIGI_DESKTOP}
    DESTINATION ${CMAKE_INSTALL_DATADIR}/applications
)
