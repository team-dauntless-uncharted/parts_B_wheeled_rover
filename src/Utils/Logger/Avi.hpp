/**
 * POSIX API対応 AVI library for Spresense
 * Header file
 * Modified from original AviLibrary to use POSIX file operations
 */

#ifndef __POSIX_AVI_LIB_CLASS_H__
#define __POSIX_AVI_LIB_CLASS_H__

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <math.h>

// 元のAviLibrary.hから移植した定数定義
#define AVI_OFFSET (240)

// 元のAviLibrary.hから移植したマクロ定義
/* Default WIDTH == 1280 (0x500) */
#define WIDTH_1 0x00
#define WIDTH_2 0x05
/* Default HEIGHT == 960 (0x3C0) */
#define HEIGHT_1 0xC0
#define HEIGHT_2 0x03

// 元のAviLibrary.hから移植した列挙型
enum AVI_REC_MODE {
  AVI_REC_MOVIE,
  AVI_REC_TIMELAPSE,
  AVI_REC_NONE
};

class PosixAviLibrary {
private:
  bool m_initialized;
  uint32_t m_movi_size;
  uint32_t m_start_time;
  uint16_t m_width;
  uint16_t m_height;
  uint32_t m_frames;
  uint32_t m_total_size;
  uint8_t  m_fps;
  uint32_t m_us_per_frame;
  uint32_t m_max_bytes_per_sec;
  float    m_duration_sec;
  AVI_REC_MODE m_rec_mode;
  
  int m_aviFile;  // POSIX file descriptor
  char m_filename[256];  // ファイル名保存用
  
  // 元のAviLibrary.hと同じヘッダーテンプレート
  char m_avi_header[AVI_OFFSET + 1];

private:
  void initializeHeader();
  off_t getAviFileSize();
  bool seekFile(off_t offset);
  bool seekToEnd();
  ssize_t writeToFile(const void* data, size_t size);
  void uint32_write_to_aviFile(uint32_t v);
  void add_frame(const char* img, uint32_t img_size);
  void write_parameters();

public:
  PosixAviLibrary();
  ~PosixAviLibrary();
  
  // 基本操作
  bool begin(const char* filename, uint16_t width, uint16_t height);
  bool writeHeader();
  void end();
  
  // 通常の動画録画
  bool startRecording();
  bool addFrame(const char* ImgBuff, uint32_t imgSize);
  bool endRecording();
  
  // タイムラプス録画
  bool startTimelapse(uint8_t target_fps);
  bool addTimelapseFrame(const char* ImgBuff, uint32_t imgSize);
  bool endTimelapse();
  
  // 情報取得用メソッド（元のライブラリと互換性維持）
  uint16_t getWidth() { return m_width; }
  uint16_t getHeight() { return m_height; }
  uint32_t getTotalFrame() { return m_frames; }
  uint32_t getFileSize() { return m_total_size; }
  uint32_t getMovieSize() { return m_movi_size; }
  uint16_t getFps() { return m_fps; }
  uint32_t getMaxDataRate() { return m_max_bytes_per_sec; }
  float getDuration() { return m_duration_sec; }
  AVI_REC_MODE getRecMode();
  
  // 値設定用メソッド（元のライブラリと互換性維持）
  void setTotalFrame(uint32_t frame) { m_frames = frame; }
  void setMovieSize(uint32_t movi_size) { m_movi_size = movi_size; }
  void setFileSize(uint32_t file_size) { m_total_size = file_size; }
  void setFps(uint8_t fps) { m_fps = fps; }
};

#endif /* __POSIX_AVI_LIB_CLASS_H__ */