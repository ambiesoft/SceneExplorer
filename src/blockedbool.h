#ifndef BLOCKEDTRUE_H
#define BLOCKEDTRUE_H


class BlockedBool
{
    volatile bool* target_;
    bool d_;
public:
    BlockedBool(volatile bool* target, bool c, volatile bool d) :
        target_(target),
        d_(d)
    {
        *target_ = c;
    }
    BlockedBool(volatile bool* target) :
        target_(target),
        d_(false)
    {
        *target_ = true;
    }
    ~BlockedBool()
    {
        *target_ = d_;
    }
};

#endif // BLOCKEDTRUE_H
