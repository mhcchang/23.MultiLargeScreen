@echo off
SetLocal EnableDelayedExpansion
(set PATH=C:\QT\QT5.12\5.12.8\MSVC2015_64\bin;!PATH!)
if defined QT_PLUGIN_PATH (
    set QT_PLUGIN_PATH=C:\QT\QT5.12\5.12.8\MSVC2015_64\plugins;!QT_PLUGIN_PATH!
) else (
    set QT_PLUGIN_PATH=C:\QT\QT5.12\5.12.8\MSVC2015_64\plugins
)
%*
EndLocal
