# CMakeの最小バージョンを指定
cmake_minimum_required(VERSION 3.10)

# プロジェクト名を設定
project(GeoUtilsTest CXX)

# C++11を有効にする
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# googletestパッケージを探す
find_package(GTest REQUIRED)

# テスト対象のソースコードがあるディレクトリをインクルードパスに追加
# このCMakeLists.txtは test/test_geoutils/ にあるため、srcディレクトリは2階層上
include_directories(../../src)

# テスト実行ファイルを定義
# テストコード(test_main.cpp)と、テスト対象のコード(GeoUtils.cpp)を指定
add_executable(
    run_tests
    test_main.cpp
    ../../src/Utils/GeoUtils/GeoUtils.cpp
)

# googletestライブラリをテスト実行ファイルにリンク
target_link_libraries(run_tests GTest::gtest_main)