#include "runtime.hpp"

void Runtime::satisfy_dep(JTaskDependency dep, uint64_t task_id) {
    // get the associated dependency queue
    std::queue<DependencyMarker> &q = dependency_hash.at(dep);

    // get the front of the dependency queue
    DependencyMarker &front = q.front();

    // make sure this is an output dependency
    assert(front.type == DependencyMarker::Type::OUT);
    // make sure the output we are waiting on is the current task
    assert(front.tid == task_id);
    // remove the dependency output
    q.pop();

    // satisfy and remove all input dependencies before the next output
    while (!q.empty() && q.front().type == DependencyMarker::Type::IN) {
        mark_satisfied( q.front() );
        q.pop();
    }
}

void Runtime::mark_satisfied(DependencyMarker &dm) {
    // get the task associated with the dependency marker
    Task &tsk = tasks[dm.rt_idx].task;
    // confirm this task matches the expected id
    assert(task.task_id == dm.tid);
    // decrement the number of unsatisfied inputs
    task.num_input_deps_unsat--;
    // if that are no more unsatisfied inputs we run the task
    if (task.num_input_deps == 0) {
        run_task(dm.rt_idx, dm.tid);
    }
}

void Runtime::register_dep(JTaskDependency dep, uint64_t task_id, uint64_t rt_idx, DependencyType type) {
    // if the dependency is not already a key, an empty queue will be created
    std::queue<DependencyMarker> &q = dependency_hash[dep];
    // create the dependency marker
    DependencyMarker dm(task_id, rt_idx, type);
    // push the dependency marker into the queue
    q.push(dm);
};
        
void Runtime::complete_task(JTask &task) {
    // satisfy each output dependency
    for (dep : task.out_dependencies) {
        satisfy_dep(dep, task.task_id);
    }

    // remove the task and its TH
    tasks[task.rt_idx].reset();
}

void Runtime::create_fiber(uint64_t loc, uint64_t tid) {
    // get the task
    JTask &s = tasks[loc].task;
    // make sure this is the task we expect
    assert(s.task_id == tid);
    
    // create the closure that will be run
    auto closure = [s, this] { s.run(); this->complete_task(s) };
    //TODO: create fiber
}

void Runtime::create_task(JTaskBuilder task_blueprint) {
    // while we can't make new tasks, yield
    while (num_tasks >= TASK_LIMIT) {
        //TODO: fiber yield
        //maybe should be task yield?
        //would need to get the running task somehow
    }

    // get the first free index
    uint64_t rt_idx = free_indices.front(); free_indices.pop();
    // get the next task id
    uint64_t task_id = nxt_task_id++;
    
    //TODO: setup the dependencies

    //make the task
    JTask task(task_id, rt_idx, work, num_input_deps, out_dependencies);
    //TODO: gonna need move

    // make the TaskHolder
    tasks[rt_idx].emplace(task);
    // mark the task as ready
    //TODO: put task in waitng bin instead
    ready_indices.push(rt_idx);
}

void Runtime::yield_task(JTask task) {
    // mark the task yielded
    tasks[task.rt_idx].status = TaskHolder::Status::YIELDED;
    // push the task into the ready indices
    ready_indices.push(task.rt_idx);
    // actually yield
    //TODO: fiber yield
}
