#pragma once

class JTaskAbstract {
    public:
        virtual void call();
        virtual ~JTaskAbstract();
        
        void yield(void);
}

typedef JTaskDependency uint8_t;

class JTask {
    private:
        const uint64_t task_id;
        const uint64_t rt_idx;
        const unique_ptr<JTaskAbstract> task;

        uint64_t num_input_deps_unsat;
        std::vector<JTaskDependency> out_dependencies;

    friend class Runtime;
}
