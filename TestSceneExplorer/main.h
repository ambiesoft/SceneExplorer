#ifndef MAIN_H
#define MAIN_H

#define EXPECT_QSTREQ(left,right) EXPECT_STREQ(QString(left).toUtf8(), QString(right).toUtf8())

#endif // MAIN_H
