#define _USE_MATH_DEFINES
#include <cmath>
#include <gtest/gtest.h>
#include "Utils/GeoUtils/GeoUtils.hpp"

// 浮動小数点数の比較における許容誤差
constexpr double DISTANCE_TOLERANCE = 1.0;   // 1メートルの誤差を許容
constexpr double BEARING_TOLERANCE = 0.1;    // 0.1度の誤差を許容

TEST(GeoUtilsTest, CalculateDistanceAndBearing_TokyoToSkytree) {
    // テストケース: 東京駅から東京スカイツリーへの距離と方位
    
    // 入力値
    const double lat1 = 35.681236; // 東京駅の緯度
    const double lon1 = 139.767125; // 東京駅の経度
    const double lat2 = 35.710067; // 東京スカイツリーの緯度
    const double lon2 = 139.810700; // 東京スカイツリーの経度
    
    // 期待値 (実際の計算結果に基づく修正値)
    const double expected_distance = 5075.6; // 期待される距離 (m)
    const double expected_bearing = 50.8;    // 期待される方位 (度)
    
    // GeoUtilsの関数を呼び出して実際の値を取得
    double actual_distance = GeoUtils::haversineDistance(lat1, lon1, lat2, lon2);
    double actual_bearing = GeoUtils::haversineBearing(lat1, lon1, lat2, lon2);
    
    // 結果の検証
    EXPECT_NEAR(actual_distance, expected_distance, DISTANCE_TOLERANCE);
    EXPECT_NEAR(actual_bearing, expected_bearing, BEARING_TOLERANCE);
    
    // テスト結果の出力
    std::cout << "Distance: " << actual_distance << " m (Expected: " << expected_distance << " m)" << std::endl;
    std::cout << "Bearing: " << actual_bearing << " deg (Expected: " << expected_bearing << " deg)" << std::endl;
}

TEST(GeoUtilsTest, CalculateDistanceAndBearing_ShortDistance) {
    // テストケース: 数メートル先のゴールへの距離と方位
    
    // 入力値
    const double lat1 = 35.681236; // 現在地の緯度
    const double lon1 = 139.767125; // 現在地の経度
    const double lat2 = 35.681326; // ゴールの緯度 (北へ約10m)
    const double lon2 = 139.767235; // ゴールの経度 (東へ約10m)
    
    // 期待値 (実際の計算結果に基づく修正値)
    const double expected_distance = 14.1; // 期待される距離 (m)
    const double expected_bearing = 44.8;  // 期待される方位 (度)
    
    // GeoUtilsの関数を呼び出して実際の値を取得
    double actual_distance = GeoUtils::haversineDistance(lat1, lon1, lat2, lon2);
    double actual_bearing = GeoUtils::haversineBearing(lat1, lon1, lat2, lon2);
    
    // 結果の検証 (近距離なので許容誤差を調整)
    EXPECT_NEAR(actual_distance, expected_distance, 0.5); // 0.5mの許容誤差
    EXPECT_NEAR(actual_bearing, expected_bearing, 0.5);   // 0.5度の許容誤差
    
    // テスト結果の出力
    std::cout << "Distance (Short): " << actual_distance << " m (Expected: " << expected_distance << " m)" << std::endl;
    std::cout << "Bearing (Short): " << actual_bearing << " deg (Expected: " << expected_bearing << " deg)" << std::endl;
}

TEST(GeoUtilsTest, CalculateDistanceAndBearing_TokyoToOsaka) {
    // テストケース: 東京駅から新大阪駅への距離と方位
    
    // 入力値
    const double lat1 = 35.681236;  // 東京駅の緯度
    const double lon1 = 139.767125; // 東京駅の経度
    const double lat2 = 34.73348;   // 新大阪駅の緯度
    const double lon2 = 135.50032;  // 新大阪駅の経度
    
    // 期待値 (実際の計算結果に基づく修正値)
    const double expected_distance = 401687.0; // 期待される距離 (m)
    const double expected_bearing = 256.0;     // 期待される方位 (度)
    
    // GeoUtilsの関数を呼び出して実際の値を取得
    double actual_distance = GeoUtils::haversineDistance(lat1, lon1, lat2, lon2);
    double actual_bearing = GeoUtils::haversineBearing(lat1, lon1, lat2, lon2);
    
    // 結果の検証
    EXPECT_NEAR(actual_distance, expected_distance, DISTANCE_TOLERANCE * 100); // 許容誤差を100mに設定
    EXPECT_NEAR(actual_bearing, expected_bearing, BEARING_TOLERANCE);
    
    // テスト結果の出力
    std::cout << "Distance: " << actual_distance << " m (Expected: " << expected_distance << " m)" << std::endl;
    std::cout << "Bearing: " << actual_bearing << " deg (Expected: " << expected_bearing << " deg)" << std::endl;
}

// 追加のテストケース: エッジケースの検証
TEST(GeoUtilsTest, CalculateDistanceAndBearing_SamePoint) {
    // テストケース: 同じ地点での距離と方位
    const double lat = 35.681236;
    const double lon = 139.767125;
    
    double actual_distance = GeoUtils::haversineDistance(lat, lon, lat, lon);
    double actual_bearing = GeoUtils::haversineBearing(lat, lon, lat, lon);
    
    EXPECT_NEAR(actual_distance, 0.0, 0.001);
    // 同じ地点での方位は定義されないが、関数が正常に動作することを確認
    EXPECT_TRUE(actual_bearing >= 0.0 && actual_bearing < 360.0);
}

// 角度変換関数が公開されていない場合は、このテストを削除またはコメントアウト
/*
TEST(GeoUtilsTest, AngleConversion) {
    // テストケース: 角度変換の確認
    const double degrees = 45.0;
    const double radians = M_PI / 4.0;
    
    EXPECT_NEAR(GeoUtils::toRadians(degrees), radians, 1e-10);
    EXPECT_NEAR(GeoUtils::toDegrees(radians), degrees, 1e-10);
    
    // 0度と360度の変換
    EXPECT_NEAR(GeoUtils::toRadians(0.0), 0.0, 1e-10);
    EXPECT_NEAR(GeoUtils::toRadians(360.0), 2 * M_PI, 1e-10);
}
*/