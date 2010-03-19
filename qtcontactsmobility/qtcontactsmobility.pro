# This is a Symbian MCL specific .pro file
# It omits most of the checks, since they are valid...

# config.pri specifies the configure options

include(config.pri)

TEMPLATE = subdirs
CONFIG+=ordered

SUBDIRS += src plugins
#built documentation snippets, if enabled
contains(build_docs, yes) {
    SUBDIRS += doc
    include(doc/doc.pri)

    OTHER_FILES += doc/src/*.qdoc doc/src/examples/*.qdoc
}

contains(build_unit_tests, yes):SUBDIRS+=tests
contains(build_examples, yes):SUBDIRS+=examples

# install Qt style headers
qtmheaders.path = $${QT_MOBILITY_INCLUDE}
qtmheaders.files = $${QT_MOBILITY_BUILD_TREE}/include/*

INSTALLS += qtmheaders
