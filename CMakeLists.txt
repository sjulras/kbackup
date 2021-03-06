cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR)

# KDE Application Version, managed by release script
set(KDE_APPLICATIONS_VERSION_MAJOR "18")
set(KDE_APPLICATIONS_VERSION_MINOR "11")
set(KDE_APPLICATIONS_VERSION_MICRO "70")
set(KDE_APPLICATIONS_VERSION "${KDE_APPLICATIONS_VERSION_MAJOR}.${KDE_APPLICATIONS_VERSION_MINOR}.${KDE_APPLICATIONS_VERSION_MICRO}")

project(kbackup)

set(QT_MIN_VERSION "5.9.0")
set(KF5_VERSION "5.46.0")

find_package(ECM ${KF5_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

add_definitions(-DTRANSLATION_DOMAIN="kbackup" -DKBACKUP_VERSION="${KDE_APPLICATIONS_VERSION}")

include(KDEInstallDirs)
include(KDEFrameworkCompilerSettings NO_POLICY_SCOPE)
include(KDECMakeSettings)
include(ECMInstallIcons)
include(FeatureSummary)

find_package(Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS
    Core
    Widgets
)

find_package(KF5 ${KF5_VERSION} REQUIRED COMPONENTS
    Notifications
    I18n
    KIO
    DocTools
    XmlGui
    IconThemes
    Archive
    WidgetsAddons
)

add_definitions(-DQT_NO_NARROWING_CONVERSIONS_IN_CONNECT)
#add_definitions(-DQT_DISABLE_DEPRECATED_BEFORE=0x060000)


add_subdirectory(src)
add_subdirectory(doc)

ki18n_install(po)
kdoctools_install(po)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
