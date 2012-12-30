import os

from build_tasks import build_file


def _run_dfs(root, acc=None):
    if acc is None:
        acc = list()

    acc.append(root)

    for n in root.edges_out:
        _run_dfs(n, acc)

    return acc


def _tasks_from(root):
    return filter(lambda n: n.is_task, _run_dfs(root))


def _file_did_change(graph, path):
    return _tasks_from(graph.file_map[path])


def _file_did_appear(graph, path):
    build_file(graph, path)
    if path not in graph.file_map:
        return []

    return _tasks_from(graph.file_map[path])


def _file_did_disappear(graph, path):
    node = graph.file_map[path]
    affected = _run_dfs(node)

    for n in affected:
        if n.is_valid:
            continue

        to_remove = (o
                     for o in n.edges_out
                     if o.is_file and os.path.exists(o.filename))

        for o in to_remove:
            os.unlink(o.filename)

        graph.remove_node(n)

    return []


def tasks_from_fs_change(graph, path):
    on_disk = os.path.exists(path)
    in_graph = path in graph.file_map

    if on_disk and in_graph:
        return _file_did_change(graph, path)
    if on_disk and not in_graph:
        return _file_did_appear(graph, path)
    if not on_disk and in_graph:
        return _file_did_disappear(graph, path)

    return []
