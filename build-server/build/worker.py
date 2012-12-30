#-*- coding: utf-8 -*-

import os

import Foundation

from build_graph import BuildGraph, recursive_build
from incremental_graph import tasks_from_fs_change

ASSET_SUBDIR = 'game-assets'
BUILD_SUBDIR = 'asset-build'


class ConstructBuildGraphOperation(Foundation.NSOperation):
    def initWithGraph_delegate_(self, graph, delegate):
        self = super(ConstructBuildGraphOperation, self).init()
        if self is None: return self

        self.graph = graph
        self.delegate = delegate

        return self

    def main(self):
        recursive_build(self.graph, os.path.join(self.graph.root, ASSET_SUBDIR))
        self.graph.load_build_cache()

        for task in filter(lambda t: t.is_output_invalid, self.graph.tasks):
            self.delegate.taskBecameOutdated_(task)


class RunTaskOperation(Foundation.NSOperation):
    def initWithGraph_task_delegate_(self, graph, task, delegate):
        self = super(RunTaskOperation, self).init()
        if self is None: return self

        self.graph = graph
        self.task = task
        self.delegate = delegate

        return self

    def main(self):
        result = self.graph.run_task(self.task)
        self.graph.save_build_cache()

        srcs = [n.filename for n in self.task.edges_in if n.is_file]
        dests = [n.filename for n in self.task.edges_out if n.is_file]

        if result:
            self.delegate.taskReportsSuccessFrom_to_(srcs, dests)
        else:
            self.delegate.taskReportsFailureFrom_to_(srcs, dests)


class RebuildFileOperation(Foundation.NSOperation):
    def initWithGraph_path_delegate_(self, graph, path, delegate):
        self = super(RebuildFileOperation, self).init()
        if self is None: return self

        self.graph = graph
        self.path = path
        self.delegate = delegate

        return self

    def main(self):
        tasks = tasks_from_fs_change(self.graph, self.path)

        for task in filter(lambda t: t.is_output_invalid, tasks):
            self.delegate.taskBecameOutdated_(task)


class Worker(Foundation.NSObject):
    def setDelegate_(self, delegate):
        self.delegate = delegate

    def setProjectRoot_(self, projectRoot):
        self.projectRoot = projectRoot
        self.graph = BuildGraph(projectRoot)

    def rebuildFileOperation_(self, path):
        return RebuildFileOperation.alloc() \
            .initWithGraph_path_delegate_(self.graph, path, self)

    def constructBuildGraphOperation(self):
        return ConstructBuildGraphOperation.alloc() \
            .initWithGraph_delegate_(self.graph, self)

# Construct Build Graph Operation Delegate

    def taskBecameOutdated_(self, task):
        task_op = RunTaskOperation.alloc() \
            .initWithGraph_task_delegate_(self.graph, task, self)

        self.delegate.worker_needsToExecuteOperation_(self, task_op)

# Task Operation Delegate

    def reportSourceFileChanged_(self, abspath):
        self.delegate.worker_reportsSourceFileChanged_(self, abspath)

    def taskReportsSuccessFrom_to_(self, sourcePaths, destinationPaths):
        self.delegate \
            .worker_reportsBuildSuccessFromSources_toDestinations_(self,
                                                                   sourcePaths,
                                                                   destinationPaths)

    def taskReportsFailureFrom_to_(self, sourcePaths, destinationPaths):
        self.delegate \
            .worker_reportsBuildFailureFromSources_toDestinations_(self,
                                                                   sourcePaths,
                                                                   destinationPaths)
