/**
 * POSIX API対応 AVI library for Spresense
 * Implementation file
 * Modified from original AviLibrary to use POSIX file operations
 */

#include "Avi.hpp"

// Spresenseのmillis()関数を外部参照
extern "C" unsigned long millis();

// Constructor
PosixAviLibrary::PosixAviLibrary() : 
  m_initialized(false),
  m_movi_size(0),
  m_start_time(0),
  m_width(0),
  m_height(0),
  m_frames(0),
  m_total_size(0),
  m_fps(0),
  m_us_per_frame(0),
  m_max_bytes_per_sec(0),
  m_duration_sec(0.0),
  m_rec_mode(AVI_REC_NONE),
  m_aviFile(-1) {
  memset(m_filename, 0, sizeof(m_filename));
  initializeHeader();
}

// Destructor
PosixAviLibrary::~PosixAviLibrary() {
  if (m_aviFile >= 0) {
    close(m_aviFile);
  }
}

// Initialize AVI header with exact values from original AviLibrary.h
void PosixAviLibrary::initializeHeader() {
  // 元のAviLibrary.hから正確なヘッダーデータをコピー
  const char header_data[AVI_OFFSET+1] = {
    0x52, 0x49, 0x46, 0x46, 0xD8, 0x01, 0x0E, 0x00, 0x41, 0x56, 0x49, 0x20, 0x4C, 0x49, 
    0x53, 0x54, 0xD0, 0x00, 0x00, 0x00, 0x68, 0x64, 0x72, 0x6C, 0x61, 0x76, 0x69, 0x68, 
    0x38, 0x00, 0x00, 0x00, 0xA0, 0x86, 0x01, 0x00, 0x80, 0x66, 0x01, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x64, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, WIDTH_1, WIDTH_2, 0x00, 0x00, 
    HEIGHT_1, HEIGHT_2, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x4C, 0x49, 0x53, 0x54, 0x84, 0x00, 
    0x00, 0x00, 0x73, 0x74, 0x72, 0x6C, 0x73, 0x74, 0x72, 0x68, 0x30, 0x00, 0x00, 0x00, 
    0x76, 0x69, 0x64, 0x73, 0x4D, 0x4A, 0x50, 0x47, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x0A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x73, 0x74, 0x72, 0x66, 0x28, 0x00, 0x00, 0x00, 
    0x28, 0x00, 0x00, 0x00, WIDTH_1, WIDTH_2, 0x00, 0x00, HEIGHT_1, HEIGHT_2, 
    0x00, 0x00, 0x01, 0x00, 0x18, 0x00, 0x4D, 0x4A, 0x50, 0x47, 0x00, 0x84, 0x03, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x4C, 0x49, 0x53, 0x54, 0x10, 0x00, 0x00, 0x00, 0x6F, 0x64, 0x6D, 0x6C, 
    0x64, 0x6D, 0x6C, 0x68, 0x04, 0x00, 0x00, 0x00, 0x64, 0x00, 0x00, 0x00, 0x4C, 0x49, 
    0x53, 0x54, 0x00, 0x01, 0x0E, 0x00, 0x6D, 0x6F, 0x76, 0x69, 0x00
  };
  
  memcpy(m_avi_header, header_data, AVI_OFFSET);
}

// Get file size using fstat
off_t PosixAviLibrary::getAviFileSize() {
  struct stat st;
  if (fstat(m_aviFile, &st) == 0) {
    return st.st_size;
  }
  return 0;
}

// Seek to specific position in file
bool PosixAviLibrary::seekFile(off_t offset) {
  return lseek(m_aviFile, offset, SEEK_SET) != -1;
}

// Seek to end of file
bool PosixAviLibrary::seekToEnd() {
  return lseek(m_aviFile, 0, SEEK_END) != -1;
}

// Write data to file
ssize_t PosixAviLibrary::writeToFile(const void* data, size_t size) {
  return write(m_aviFile, data, size);
}

// Write 32-bit value in little-endian format
void PosixAviLibrary::uint32_write_to_aviFile(uint32_t v) {
  char value = v % 0x100;
  writeToFile(&value, 1);  v = v >> 8; 
  value = v % 0x100;
  writeToFile(&value, 1);  v = v >> 8;
  value = v % 0x100;
  writeToFile(&value, 1);  v = v >> 8; 
  value = v;
  writeToFile(&value, 1); 
}

// Add a frame to the AVI file
void PosixAviLibrary::add_frame(const char* img, uint32_t img_size) {
  seekToEnd();
  writeToFile("00dc", 4);
  uint32_write_to_aviFile(img_size);
  writeToFile(img, img_size);
  m_movi_size += img_size;
  ++m_frames;  
}

// Write parameters to AVI header
void PosixAviLibrary::write_parameters() {
  /* overwrite riff file size */
  seekFile(0x04);
  uint32_write_to_aviFile(m_total_size);

  /* overwrite hdrl */
  /* hdrl.avih.us_per_frame */
  seekFile(0x20);
  uint32_write_to_aviFile(m_us_per_frame);
  seekFile(0x24);
  uint32_write_to_aviFile(m_max_bytes_per_sec);

  /* hdrl.avih.tot_frames */
  seekFile(0x30);
  uint32_write_to_aviFile(m_frames);
  seekFile(0x84);
  uint32_write_to_aviFile(m_fps);

  /* hdrl.strl.list_odml.frames */
  seekFile(0xe0);
  uint32_write_to_aviFile(m_frames);
  seekFile(0xe8);
  uint32_write_to_aviFile(m_movi_size);  
}

// Initialize the library with file and dimensions
bool PosixAviLibrary::begin(const char* filename, uint16_t width, uint16_t height) {
  if (!filename || width == 0 || height == 0) {
    return false;
  }
  
  strncpy(m_filename, filename, sizeof(m_filename) - 1);
  m_filename[sizeof(m_filename) - 1] = '\0';
  
  m_aviFile = open(filename, O_RDWR | O_CREAT | O_TRUNC, 0666);
  if (m_aviFile < 0) {
    return false;
  }

  m_width = width;
  m_height = height;
  m_frames = 0;
  m_movi_size = 0;
  m_total_size = 0;
  m_initialized = true;
  return true;
}

// Write AVI header to file
bool PosixAviLibrary::writeHeader() {
  if (!m_initialized || m_frames != 0) {
    return false;
  }
  
  uint8_t width_1   = (uint8_t)( m_width  & 0x00ff); 
  uint8_t width_2   = (uint8_t)((m_width  & 0xff00) >> 8); 
  uint8_t height_1  = (uint8_t)( m_height & 0x00ff);
  uint8_t height_2  = (uint8_t)((m_height & 0xff00) >> 8); 
  
  m_avi_header[ 64] = width_1;
  m_avi_header[ 65] = width_2;
  m_avi_header[ 68] = height_1;
  m_avi_header[ 69] = height_2;
  m_avi_header[168] = width_1;
  m_avi_header[169] = width_2;
  m_avi_header[172] = height_1;
  m_avi_header[173] = height_2;
  
  return writeToFile(m_avi_header, AVI_OFFSET) == AVI_OFFSET;
}

// Start recording
bool PosixAviLibrary::startRecording() {
  if (!m_initialized || m_aviFile < 0) {
    return false;
  }
  
  if (!writeHeader()) {
    return false;
  }
  
  m_rec_mode = AVI_REC_MOVIE;
  m_start_time = millis();
  return true;
}

// Add a frame to the recording
bool PosixAviLibrary::addFrame(const char* ImgBuff, uint32_t imgSize) {
  if (!m_initialized || !ImgBuff || !imgSize) {
    return false;
  }

  add_frame(ImgBuff, imgSize);

  m_duration_sec = (millis() - m_start_time) / 1000.0f;
  if (m_duration_sec > 0) {
    float fps_in_float = m_frames / m_duration_sec;
    float us_per_frame_in_float = 1000000.0f / fps_in_float;
    m_fps = round(fps_in_float);  // 元のコードと同じround()関数を使用
    m_us_per_frame = round(us_per_frame_in_float);  // 元のコードと同じround()関数を使用
    m_total_size = m_movi_size + 12 * m_frames + 4;
    if (m_frames > 0) {
      m_max_bytes_per_sec = m_movi_size * m_fps / m_frames;
    }
  }
  return true;
}

// End recording
bool PosixAviLibrary::endRecording() {
  if (!m_initialized || m_aviFile < 0) {
    return false;
  }
  write_parameters();
  return true;
}

// Start timelapse recording
bool PosixAviLibrary::startTimelapse(uint8_t target_fps) {
  if (!m_initialized || !target_fps) {
    return false;
  }
  
  m_fps = target_fps;
  m_rec_mode = AVI_REC_TIMELAPSE;
  return writeHeader();
}

// Add frame to timelapse
bool PosixAviLibrary::addTimelapseFrame(const char* ImgBuff, uint32_t imgSize) {
  if (!m_initialized || !ImgBuff || !imgSize) {
    return false;
  }

  add_frame(ImgBuff, imgSize);

  m_duration_sec = (float)m_frames / m_fps;
  m_us_per_frame = round(1000000.0f / m_fps);  // 元のコードと同じround()関数を使用
  m_total_size = m_movi_size + 12 * m_frames + 4;
  if (m_frames > 0) {
    m_max_bytes_per_sec = m_movi_size * m_fps / m_frames;
  }
  return true;
}

// End timelapse recording
bool PosixAviLibrary::endTimelapse() {
  if (!m_initialized || m_aviFile < 0) {
    return false;
  }
  write_parameters();
  return true;
}

// Get recording mode
AVI_REC_MODE PosixAviLibrary::getRecMode() {
  return m_rec_mode;
}

// Clean up resources
void PosixAviLibrary::end() {
  if (m_aviFile >= 0) {
    close(m_aviFile);
    m_aviFile = -1;
  }
  m_initialized = false;
  m_rec_mode = AVI_REC_NONE;
}