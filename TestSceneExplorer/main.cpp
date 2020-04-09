#include <QString>

#include "gtest/gtest.h"

#include "../../lsMisc/stdQt/stdQt.h"

using namespace AmbiesoftQt;

#define EXPECT_QSTREQ(left,right) EXPECT_STREQ(QString(left).toUtf8(), QString(right).toUtf8())

TEST(stdQt,RemoveExtensionFromPath)
{
    QString path = "";
    EXPECT_TRUE(RemoveExtensionFromPath(nullptr).isEmpty());
    EXPECT_TRUE(RemoveExtensionFromPath("").isEmpty());
    EXPECT_TRUE(RemoveExtensionFromPath(" ").isEmpty());
    EXPECT_TRUE(RemoveExtensionFromPath(".").isEmpty());
    EXPECT_TRUE(RemoveExtensionFromPath(" . ").isEmpty());
    EXPECT_TRUE(RemoveExtensionFromPath(".aaa").isEmpty());
    EXPECT_TRUE(RemoveExtensionFromPath("..aaa").isEmpty());
    EXPECT_TRUE(RemoveExtensionFromPath(" ..aaa").isEmpty());


    EXPECT_QSTREQ(RemoveExtensionFromPath("aaa"),"aaa");
    EXPECT_QSTREQ(RemoveExtensionFromPath("/a/a"),"a");
    EXPECT_QSTREQ(RemoveExtensionFromPath("/a/a.b..") ,"a.b");
    EXPECT_QSTREQ(RemoveExtensionFromPath("C:\\aaa\bbb\\") ,"");
    EXPECT_QSTREQ(RemoveExtensionFromPath("C:\\aaa\bbb\\t t t") ,"t t t");
    EXPECT_QSTREQ(RemoveExtensionFromPath("C:\\aaa\bbb\\t t t ") ,"");
    EXPECT_QSTREQ(RemoveExtensionFromPath("C:\\aaa\bbb\\t t t.mp4") ,"t t t");
}
