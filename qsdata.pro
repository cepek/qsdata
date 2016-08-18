lessThan(QT_MAJOR_VERSION, 5): error("requires Qt 5 or later")

TEMPLATE = subdirs

SUBDIRS += \
    qsdata \
    qsdlib

qsdata.depends = qsdlib
