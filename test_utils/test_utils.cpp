#include "gtest/gtest.h"
#include "NonOwningPtr.hpp"
#include "owned_pointer.h"
#include "logger.h"


struct Dummy
{
    Dummy(int i) : i{i} {}
    int i;
    virtual ~Dummy() = default;
};

using namespace testutils;
using namespace testing;


class NonOwningPtrTest : public Test
{
public:
    NonOwningPtr<Dummy> ptr = make_non_owning<Dummy>(4);
};

TEST_F(NonOwningPtrTest, operators)
{
    EXPECT_EQ(4, ptr->i);
    EXPECT_EQ(4, (*ptr).i);
    EXPECT_TRUE(ptr);
}

TEST_F(NonOwningPtrTest, ownershipRelease)
{
    std::unique_ptr<Dummy> ptrNew = ptr.unique_ptr();
    EXPECT_EQ(4, ptr->i);
    EXPECT_EQ(4, (*ptr).i);
    EXPECT_TRUE(ptr);
    EXPECT_THROW(ptr.unique_ptr(), NullPtr);
}

TEST_F(NonOwningPtrTest, ownershipReleaseObjectDestruction)
{
    auto ptrNew = ptr.unique_ptr();
    ptrNew.reset();
    EXPECT_THROW(*ptr, NullPtr);
    EXPECT_THROW((void)ptr->i, NullPtr);
    EXPECT_FALSE(ptr);
}
