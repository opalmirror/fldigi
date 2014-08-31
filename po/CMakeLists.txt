file(GLOB FLDIGI_PO_FILES "*.po")
gettext_create_translations(fldigi.pot ALL ${FLDIGI_PO_FILES})
