/*
 * Copyright (c) 2020-2021 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef PROJMGRTESTENV_H
#define PROJMGRTESTENV_H

#include "gtest/gtest.h"

#include <list>

extern std::string testoutput_folder;
extern std::string testinput_folder;
extern std::string testcmsispack_folder;
extern std::string testcmsiscompiler_folder;
extern std::string schema_folder;

/**
 * @brief direct console output to string
*/
class StdStreamRedirect {
public:
  StdStreamRedirect();
  ~StdStreamRedirect();

  std::string GetOutString();
  std::string GetErrorString();
  void ClearStringStreams();

private:
  std::stringstream m_outbuffer;
  std::stringstream m_cerrbuffer;
  std::streambuf*   m_stdoutStreamBuf;
  std::streambuf*   m_stdcerrStreamBuf;
};

/**
 * @brief global test environment for all the test suites
*/
class ProjMgrTestEnv : public ::testing::Environment {
public:
  void SetUp() override;
  void TearDown() override;
  static void CompareFile(const std::string& file1, const std::string& file2);
  static const std::string& GetCmsisPackRoot();
  static std::list<std::string> GetInstalledPdscFiles(bool bLatestsOnly = false);
  static std::string GetFilteredPacksString(const std::list<std::string>& pdscFiles, const std::string& includeIds);
  static bool FilterId(const std::string& id, const std::string& includeIds);
};

#endif  // PROJMGRTESTENV_H
