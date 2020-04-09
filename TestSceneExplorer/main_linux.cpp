#include <QString>

#include "gtest/gtest.h"

#include "../../lsMisc/stdQt/stdQt.h"

#include "main.h"
using namespace AmbiesoftQt;



TEST(stdQt,RemoveExtensionFromPathLinux)
{
    QString path = "";
    EXPECT_TRUE(RemoveExtensionFromPath(nullptr).isEmpty());
    EXPECT_TRUE(RemoveExtensionFromPath("").isEmpty());
    EXPECT_TRUE(!RemoveExtensionFromPath(" ").isEmpty());
    EXPECT_TRUE(RemoveExtensionFromPath(".").isEmpty());
    EXPECT_TRUE(RemoveExtensionFromPath(" . ").isEmpty());
    EXPECT_TRUE(RemoveExtensionFromPath(".aaa").isEmpty());
    EXPECT_TRUE(RemoveExtensionFromPath("..aaa").isEmpty());
    EXPECT_TRUE(RemoveExtensionFromPath(" ..aaa").isEmpty());


    EXPECT_QSTREQ(RemoveExtensionFromPath("aaa"),"aaa");
    EXPECT_QSTREQ(RemoveExtensionFromPath("/a/a"),"a");
    EXPECT_QSTREQ(RemoveExtensionFromPath("/a/a.b..") ,"a.b");
    EXPECT_QSTREQ(RemoveExtensionFromPath("/a/a.b..mp4") ,"a.b");
    EXPECT_QSTREQ(RemoveExtensionFromPath("/a/a.b.mp4") ,"a.b");
}
