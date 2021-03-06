# SPDX-License-Identifier: Apache-2.0
#
# Copyright (C) 2015, ARM Limited and contributors.
#
# Licensed under the Apache License, Version 2.0 (the "License"); you may
# not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

import logging

""" Helper module for Analysis classes """


class AnalysisModule(object):
    """
    Base class for Analysis modules.

    :param trace: input Trace object
    :type trace: :mod:`libs.utils.Trace`
    """

    def __init__(self, trace):

        self._log = logging.getLogger('Analysis')

        self._trace = trace
        self._platform = trace.platform
        self._data_dir = trace.data_dir

        self._dfg_trace_event = trace._dfg_trace_event

        trace._registerDataFrameGetters(self)

        # Further initialization not possible if platform info is missing
        if not self._platform:
            return

        # By default assume SMP system
        self._big_cap = 1024
        self._little_cap = 1024
        self._big_cpus = range(self._platform['cpus_count'])
        self._little_cpus = []

        if self._trace.has_big_little:
            self._little_cap = self._platform['nrg_model']['little']['cpu']['cap_max']
            self._big_cpus = self._platform['clusters']['big']
            self._little_cpus = self._platform['clusters']['little']

# vim :set tabstop=4 shiftwidth=4 expandtab
