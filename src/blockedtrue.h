#ifndef BLOCKEDTRUE_H
#define BLOCKEDTRUE_H


class BlockedTrue
{
    bool* value_;
public:
    BlockedTrue(bool* v) : value_(v)
    {
        *value_ = true;
    }
    ~BlockedTrue()
    {
        *value_ = false;
    }
};

#endif // BLOCKEDTRUE_H
