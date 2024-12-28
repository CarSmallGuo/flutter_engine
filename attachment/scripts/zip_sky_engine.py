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
import zipfile
import logging
from pathlib import Path
from utils import getArch

log = logging.getLogger(__name__)


def genZipFile():
  engine_project_root_dir = Path(os.path.realpath(__file__)).parents[4]
  sky_engine_path = engine_project_root_dir.joinpath("src/out/host_release/gen/dart-pkg/sky_engine")
  host_release_path = engine_project_root_dir.joinpath("src/out/host_release/gen/dart-pkg")
  arch = getArch()
  with zipfile.ZipFile(engine_project_root_dir.joinpath('sky_engine.zip'), 'w',
                       zipfile.ZIP_DEFLATED) as zipf:
    for entry in sky_engine_path.rglob("*"):
      if (entry.is_dir()):
        continue
      elif (entry.is_symlink()):
        link = str(entry)
        real = str(entry.readlink())
        print(f'found sym_link: {link} -> {real}')
        zipf.write(real, entry.relative_to(host_release_path))
      else:
        print(entry.relative_to(host_release_path))
        zipf.write(entry, entry.relative_to(host_release_path))


def main():
  genZipFile()


if __name__ == "__main__":
  exit(main())
