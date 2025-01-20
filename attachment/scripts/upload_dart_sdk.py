#!/usr/bin/env python3
# coding=utf-8
#
# Copyright (c) 2021-2024 Huawei Device Co., Ltd.
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import os
import logging
import argparse
import platform
from pathlib import Path
from utils import getArch, getEnginePath, runGitCommand, upload

engine_path = getEnginePath()

logging.basicConfig()
log = logging.getLogger()
log.setLevel(logging.INFO)


def main():
  parser = argparse.ArgumentParser(
      prog='upload_dart_sdk', description='upload dart sdk', epilog='upload dart sdk'
  )
  parser.add_argument('-t', '--tag')
  parser.add_argument('-f', '--file')
  parser.add_argument('--arch', type=str, choices=['x64', 'arm64'], default="x64")
  args = parser.parse_args()
  osArch = args.arch
  osName = platform.system().lower()
  file_name = f'dart-sdk-{osName}-{osArch}.zip'
  if args.file:
    file_path = args.file
  else:
    file_path = Path(engine_path).joinpath(file_name).__str__()
  flutter_path = Path(engine_path).joinpath('src/flutter').__str__()
  if args.tag:
    version = args.tag
  else:
    version = runGitCommand(f'git -C {flutter_path} rev-parse HEAD')
  log.info(version)
  upload(version, file_path)


if __name__ == "__main__":
  exit(main())
