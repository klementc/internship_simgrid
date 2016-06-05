#pragma once

class ElasticTask {
    private:

    public:
        ElasticTask(task);
        setTriggerRatioVariation(vector<date, ratio>);
        setTriggerTrace(string fileName);
        addOutputStream(ElasticTask e2);
        modifyTask(task);
        updateTriggerRatioVariation(vector<date, ratio>);
        setRatioVariation(date, ratio);
        triggerOneTime();
}
