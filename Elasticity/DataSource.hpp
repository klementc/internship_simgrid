#ifndef DATASOURCE_HPP
#define DATASOURCE_HPP

#include "ElasticConfig.hpp"
#include "ElasticTask.hpp"
#include "RequestType.hpp"

class DataSource {
  private:
    /**
     * The name of the mailbox to which requests will be sent
     */
    std::string mbName_;
    bool keepGoing_;
    RequestType rt_;
    /**
     * Returns the next timestamps at which the node should send a request
     * implement this method with your own data production scheme
     */
    virtual double getNextReqTS() = 0;
    //virtual double getNextFlopAmount() = 0;
    virtual uint64_t getNextReqSize() = 0;
  public:
    DataSource(std::string mbName, RequestType rt)
      : mbName_(mbName), keepGoing_(true), rt_(rt) {};

    DataSource(std::string mbName)
      : DataSource(mbName_, RequestType::DEFAULT){};
    void run();
    void suspend();
};

/**
 * Example of a custom dataSource
 *  -> Requests are triggered at a fixed interval
 *  -> Requests size is fixed
 */
class DataSourceFixedInterval : public DataSource {
  private:
    double interval_;
    uint64_t reqSize_;
    double previousTrigger_;
  public:
    DataSourceFixedInterval(std::string mbName, RequestType rt, double interval, uint64_t reqS)
      :DataSource(mbName, rt), interval_(interval), previousTrigger_(0) {};
    DataSourceFixedInterval(std::string mbName, double interval, uint64_t reqS)
      :DataSourceFixedInterval(mbName, RequestType::DEFAULT, interval_, reqS) {};
    virtual inline uint64_t getNextReqSize() {return reqSize_;};
    virtual double getNextReqTS();
};

/**
 * Example of a custom dataSource
 *  -> Requests are triggered given a timestamp file
 *  -> Fixed requests size
 * The file should containe one timestamp per line (first line ignored to allow for csv headers)
 */
class DataSourceTSFile : public DataSource {
  private:
    std::ifstream file_;
    uint64_t reqSize_;
  public:
    DataSourceTSFile(std::string mbName, RequestType rt, std::string fName, uint64_t reqSize)
      :DataSource(mbName, rt), reqSize_(reqSize){file_.open(fName);std::string a;file_>>a;};
    DataSourceTSFile(std::string mbName, std::string fName, uint64_t reqSize)
      :DataSourceTSFile(mbName, RequestType::DEFAULT, fName, reqSize){}
    inline uint64_t getNextReqSize() {return reqSize_;};
    virtual double getNextReqTS();
};
#endif