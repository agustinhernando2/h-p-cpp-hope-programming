#include <gtest/gtest.h>
#include <supplies_data_handler.hpp>
#include <filesystem>
#include <iostream>

const std::string database = "data/database_test.db";

class SuppliesDataHandlerTest : public ::testing::Test {
protected:
    void SetUp() override {
        if (!std::filesystem::exists("data")) {
            std::filesystem::create_directories("data");
        }
    }

    void TearDown() override {
        std::filesystem::remove_all("data");
    }
};

// TEST_F is to use setup and teardown methods 
TEST_F(SuppliesDataHandlerTest, SetSupplyFromDatabase) {
    std::string message = "{\"supply_name\":\"test\",\"supply_amount\":\"1\",\"supply_type\":\"test\"}";
    set_supply_from_database(message, database);
    std::string supplies = get_supplies_from_database(database);

    nlohmann::json expected;
    expected["test"]["test"] = 1;
    std::string expected_str = expected.dump(4);
    EXPECT_EQ(supplies, expected_str);
}

TEST(ExampleTest, Example) {
    EXPECT_EQ(1, 1);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
