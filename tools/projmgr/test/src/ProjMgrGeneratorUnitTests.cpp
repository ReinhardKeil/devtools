/*
 * Copyright (c) 2020-2021 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ProjMgr.h"
#include "ProjMgrTestEnv.h"
#include "RteFsUtils.h"
#include "gtest/gtest.h"
#include <regex>
#include <filesystem>

using namespace std;

class ProjMgrGeneratorUnitTests : public ProjMgrGenerator, public ::testing::Test {
protected:
  ProjMgrGeneratorUnitTests() {}
  virtual ~ProjMgrGeneratorUnitTests() {}
};

TEST_F(ProjMgrGeneratorUnitTests, GetStringFromVector) {
  string expected = "Word1 Word2 Word3";
  const vector<string> vec = {
   "Word1", "Word2", "Word3",
  };
  EXPECT_EQ(expected, ProjMgrGenerator().GetStringFromVector(vec, " "));

  const vector<string> emptyVec;
  EXPECT_EQ("", ProjMgrGenerator().GetStringFromVector(emptyVec, " "));
}

TEST_F(ProjMgrGeneratorUnitTests, GetLocalTimestamp) {
  string timestamp = ProjMgrGenerator().GetLocalTimestamp();
  EXPECT_TRUE(regex_match(timestamp,
  std::regex("^([0-9]){4}(-([0-9]){2}){2}T([0-9]){2}(:([0-9]){2}){2}$")));
}

TEST_F(ProjMgrGeneratorUnitTests, GenDir) {
  char* argv[6];

  const string& csolution = testinput_folder + "/TestSolution/gendir.csolution.yml";
  argv[1] = (char*)"run";
  argv[2] = (char*)"--solution";
  argv[3] = (char*)csolution.c_str();
  argv[4] = (char*)"-g";
  argv[5] = (char*)"RteTestGeneratorIdentifier";

  EXPECT_EQ(0, ProjMgr::RunProjMgr(6, argv, 0));

  const string generatorInputFile = testinput_folder + "/TestSolution/TestProject3/TestProject3.Debug+TypeA.cbuild.yml";
  const string generatedGPDSC = testinput_folder + "/TestSolution/TestProject3/gendir/RteTestGen_ARMCM0/RteTest.gpdsc";

  EXPECT_EQ(true, std::filesystem::exists(generatorInputFile));
  EXPECT_EQ(true, std::filesystem::exists(generatedGPDSC));
}

TEST_F(ProjMgrGeneratorUnitTests, GenFiles) {
  char* argv[6];

  const string& csolution = testinput_folder + "/TestSolution/genfiles.csolution.yml";
  argv[1] = (char*)"run";
  argv[2] = (char*)"--solution";
  argv[3] = (char*)csolution.c_str();
  argv[4] = (char*)"-g";
  argv[5] = (char*)"RteTestGeneratorIdentifier";

  EXPECT_EQ(0, ProjMgr::RunProjMgr(6, argv, 0));

  const string generatorInputFile = testinput_folder + "/TestSolution/TestProject3_1/TestProject3_1.Debug+TypeA.cbuild.yml";
  const string generatedGPDSC = testinput_folder + "/TestSolution/TestProject3_1/gendir/RteTestGen_ARMCM0/RteTest.gpdsc";

  ProjMgrTestEnv::CompareFile(testinput_folder + "/TestSolution/ref/TestProject3_1.Debug+TypeA.cbuild.yml",  generatorInputFile);

  EXPECT_EQ(true, std::filesystem::exists(generatorInputFile));
  EXPECT_EQ(true, std::filesystem::exists(generatedGPDSC));
}

TEST_F(ProjMgrGeneratorUnitTests, NoExeFiles) {
  char* argv[6];

  StdStreamRedirect streamRedirect;
  const string& csolution = testinput_folder + "/TestSolution/gen_noexe.csolution.yml";
  argv[1] = (char*)"run";
  argv[2] = (char*)"--solution";
  argv[3] = (char*)csolution.c_str();
  argv[4] = (char*)"-g";
  argv[5] = (char*)"RteTestGeneratorNoExe";

  // execution fails
  EXPECT_EQ(1, ProjMgr::RunProjMgr(6, argv, 0));

  auto errStr = streamRedirect.GetErrorString();
  EXPECT_NE(string::npos, errStr.find("permissions"));
  // still cbuild.yaml file got created
  const string generatorInputFile = testinput_folder + "/TestSolution/TestProject3_2/TestProject3_2.Debug+TypeA.cbuild.yml";
  EXPECT_TRUE(std::filesystem::exists(generatorInputFile));
  const string generatedGPDSC = testinput_folder + "/TestSolution/TestProject3_2/gendir/RteTestGen_ARMCM0/RteTest.gpdsc";
  // but not gpdsc
  EXPECT_FALSE(std::filesystem::exists(generatedGPDSC));
}

TEST_F(ProjMgrGeneratorUnitTests, DryRunIncapableGenerator) {
  char* argv[7];

  StdStreamRedirect streamRedirect;
  const string& csolution = testinput_folder + "/TestSolution/gen_nodryrun.csolution.yml";
  argv[1] = (char*)"run";
  argv[2] = (char*)"--solution";
  argv[3] = (char*)csolution.c_str();
  argv[4] = (char*)"-g";
  argv[5] = (char*)"RteTestGeneratorNoDryRun";
  argv[6] = (char*)"--dry-run";

  EXPECT_NE(0, ProjMgr::RunProjMgr(7, argv, 0));
  auto outStr = streamRedirect.GetErrorString();
  EXPECT_NE(string::npos, outStr.find("is not dry-run capable"));
}

TEST_F(ProjMgrGeneratorUnitTests, DryRun) {
  char* argv[7];

  StdStreamRedirect streamRedirect;
  const string& csolution = testinput_folder + "/TestSolution/genfiles.csolution.yml";
  argv[1] = (char*)"run";
  argv[2] = (char*)"--solution";
  argv[3] = (char*)csolution.c_str();
  argv[4] = (char*)"-g";
  argv[5] = (char*)"RteTestGeneratorIdentifier";
  argv[6] = (char*)"--dry-run";

  const string generatorInputFile = testinput_folder + "/TestSolution/TestProject3_1/TestProject3_1.Debug+TypeA.cbuild.yml";
  const string targetGPDSC = testinput_folder + "/TestSolution/TestProject3_1/gendir/RteTestGen_ARMCM0/RteTest.gpdsc";

  RteFsUtils::RemoveFile(targetGPDSC);

  EXPECT_EQ(0, ProjMgr::RunProjMgr(7, argv, 0));

  ProjMgrTestEnv::CompareFile(testinput_folder + "/TestSolution/ref/TestProject3_1.Debug+TypeA.cbuild.yml",  generatorInputFile);

  EXPECT_EQ(true, std::filesystem::exists(generatorInputFile));
  EXPECT_EQ(false, std::filesystem::exists(targetGPDSC));

  // Expect that the GPDSC content was printed to stdout, enclosed within the begin and end marks
  auto outStr = streamRedirect.GetOutString();
  string beginGpdscMark = "-----BEGIN GPDSC-----\n";
  string endGpdscMark = "-----END GPDSC-----\n";
  auto beginGpdscMarkIndex = outStr.find(beginGpdscMark);
  auto endGpdscMarkIndex = outStr.find(endGpdscMark);
  EXPECT_NE(string::npos, beginGpdscMarkIndex);
  EXPECT_NE(string::npos, endGpdscMarkIndex);
  auto gpdscContentIndex = beginGpdscMarkIndex + beginGpdscMark.size();
  auto contentLength = endGpdscMarkIndex - gpdscContentIndex;
  string gpdscContent = outStr.substr(gpdscContentIndex, contentLength);

  // Check that the GPDSC content seems OK (the full reference GPDSC file is not easily available from the test for comparison)
  EXPECT_EQ(0, gpdscContent.find("<?xml"));
  EXPECT_NE(string::npos, gpdscContent.find("<component generator=\"RteTestGeneratorIdentifier\""));
}
