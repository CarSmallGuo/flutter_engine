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

import logging
import argparse
from pathlib import Path
from utils import getEnginePath, runGitCommand
from utils import upload

engine_path = getEnginePath()

logging.basicConfig()
log = logging.getLogger()
log.setLevel(logging.INFO)


def main():
  parser = argparse.ArgumentParser(
      prog='upload sky_engine', description='upload sky_engine', epilog='upload sky_engine'
  )
  parser.add_argument('-t', '--tag')
  parser.add_argument('-f', '--file')
  args = parser.parse_args()
  file_name = 'sky_engine.zip'
  if args.file:
    file_path = args.file
  else:
    file_path = Path(engine_path).joinpath(file_name).__str__()
  if args.tag:
    version = args.tag
  else:
    flutter_path = Path(engine_path).joinpath('src/flutter').__str__()
    version = runGitCommand(f'git -C {flutter_path} rev-parse HEAD')
  log.info(version)
  upload(version, file_path)


if __name__ == "__main__":
  exit(main())
