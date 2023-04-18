# 含多个子项目的项目文件
TEMPLATE = subdirs
# 指定子项目的路径
SUBDIRS += \
    src/FluentUI/FluentUI.pro \
    src/OwlLive
    src/OwlLive.depends = src/FluentUI/FluentUI.pro \

