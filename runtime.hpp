#pragma once

class Runtime {
    struct DependencyMarker {
        enum class Type {
            IN, OUT
        }

        const Type type;
        const uint64_t tid;
        const uint64_t rt_idx;
    }

    struct TaskHolder {
        enum class Status {
            NOT_STARTED, RUNNING, YIELDED, COMPLETED
        }

        Status status;
        JTask task;
        std::optional<boost::fibers::fiber>;
    }

    std::vector< std::option<TaskHolder> > tasks;
    uint64_t num_tasks;
    uint64_t nxt_task_id;
    std::queue<uint64_t> free_indices;
    std::queue<uint64_t> ready_indices;
    std::unordered_map< uint8_t*, std::queue<DependencyMarker> > dependency_hash;
    
    void satisfy_dep(JTaskDependency dep, uint64_t task_id);
    void complete_task(JTask task);
    void create_fiber(uint64_t loc);
    void register_dep(JTaskDependency dep, uint64_t task_id, uint64_t location);

    public:
        void create_task(JTaskBuilder task_blueprint);
        
        // for the JTaskAbstract class
        void yield_task(JTask task);

        //for the JTaskBuilder class
        void get_task_id();
}
