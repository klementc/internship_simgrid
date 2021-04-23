
#include <simgrid/s4u/Actor.hpp>
#include <simgrid/s4u/Host.hpp>
#include <simgrid/s4u/Mailbox.hpp>
#include <simgrid/s4u/Engine.hpp>
#include <simgrid/s4u/Comm.hpp>
#include "ElasticPolicy.hpp"
#include "ElasticTask.hpp"
#include "DataSource.hpp"
#include <memory>

XBT_LOG_NEW_DEFAULT_CATEGORY(run_log, "logs of the experiment");
/* RETURN FUNCTIONS, AUTO GENERATED CODE, MODIFY IF YOU KNOW WHAT YOU WANT */
    void return_nginx_web_server(TaskDescription* td) {
      XBT_DEBUG("Return function of service nginx_web_server");
      switch (td->requestType)
      {
case RequestType::COMPOSE:
      if(td->nbHops == 1){
        XBT_DEBUG("Output Function for nginx_web_server (PARALLEL CHILDS)");

          XBT_DEBUG("Put to home_timeline_service");
          s4u_Mailbox* m_home_timeline_service = s4u_Mailbox::by_name("home_timeline_service");
          TaskDescription* td_0 = new TaskDescription(*td);
          td_0->requestType = RequestType::COMPOSE_0;
          m_home_timeline_service->put(td_0, td_0->dSize);

          XBT_DEBUG("Put to post_storage_service");
          s4u_Mailbox* m_post_storage_service = s4u_Mailbox::by_name("post_storage_service");
          TaskDescription* td_1 = new TaskDescription(*td);
          td_1->requestType = RequestType::COMPOSE_1;
          m_post_storage_service->put(td_1, td_1->dSize);

}
break;
	}
}
    void return_home_timeline_service(TaskDescription* td) {
      XBT_DEBUG("Return function of service home_timeline_service");
      switch (td->requestType)
      {
case RequestType::COMPOSE_0:
      if(td->nbHops == 2){
        XBT_DEBUG("Output Function for home_timeline_service (PARALLEL CHILDS)");

          XBT_DEBUG("Put to test");
          s4u_Mailbox* m_test = s4u_Mailbox::by_name("test");
          TaskDescription* td_0 = new TaskDescription(*td);
          td_0->requestType = RequestType::COMPOSE_0_0;
          m_test->put(td_0, td_0->dSize);

          XBT_DEBUG("Put to test3");
          s4u_Mailbox* m_test3 = s4u_Mailbox::by_name("test3");
          TaskDescription* td_1 = new TaskDescription(*td);
          td_1->requestType = RequestType::COMPOSE_0_1;
          m_test3->put(td_1, td_1->dSize);

}
break;
	}
}
    void return_test(TaskDescription* td) {
      XBT_DEBUG("Return function of service test");
      switch (td->requestType)
      {

case RequestType::COMPOSE_0_0:
    if(td->nbHops == 3){
      XBT_DEBUG("Output Function test (FINAL NODE, NO CHILD!)");
      // TODO DELETE
    for (auto it = td->parentSpans.rbegin(); it != td->parentSpans.rend(); ++it)
  {
    if((*it)->get() != NULL){
    //XBT_DEBUG("%lf %lf",simgrid::s4u::Engine::get_instance()->get_clock(),simgrid::s4u::Engine::get_instance()->get_clock()*1000000 );
    auto t2 = std::chrono::seconds(946684800)+std::chrono::microseconds(int(simgrid::s4u::Engine::get_instance()->get_clock()*1000000));
    (*it)->get()->Log({{"end",t2.count()}});
    (*it)->get()->Finish({opentracing::v3::FinishTimestamp(t2)});
    (*it)->reset();
    (*it) == NULL;
    }
  }

    }

break;


case RequestType::COMPOSE_1:
      if(td->nbHops == 3){
        XBT_DEBUG("Output Function for COMPOSE_1, Final service, DELETE");
      // TODO DELETE
    for (auto it = td->parentSpans.rbegin(); it != td->parentSpans.rend(); ++it)
  {
    //XBT_DEBUG("%lf %lf",simgrid::s4u::Engine::get_instance()->get_clock(),simgrid::s4u::Engine::get_instance()->get_clock()*1000000 );
    auto t2 = std::chrono::seconds(946684800)+std::chrono::microseconds(int(simgrid::s4u::Engine::get_instance()->get_clock()*1000000));
    if ((*it)->get() != NULL){
    (*it)->get()->Log({{"end",t2.count()}});
    (*it)->get()->Finish({opentracing::v3::FinishTimestamp(t2)});
    (*it)->reset();
    (*it) = NULL;
    }
  }


        // TODO DELETE JAEGER SPANS
      }

break;
	}
}
    void return_test3(TaskDescription* td) {
      XBT_DEBUG("Return function of service test3");
      switch (td->requestType)
      {

case RequestType::COMPOSE_0_1:
    if(td->nbHops == 3){
      XBT_DEBUG("Output Function test3 (FINAL NODE, NO CHILD!)");
            // TODO DELETE
    for (auto it = td->parentSpans.rbegin(); it != td->parentSpans.rend(); ++it)
  {
        if ((*it)->get() != NULL){
    //XBT_DEBUG("%lf %lf",simgrid::s4u::Engine::get_instance()->get_clock(),simgrid::s4u::Engine::get_instance()->get_clock()*1000000 );
    auto t2 = std::chrono::seconds(946684800)+std::chrono::microseconds(int(simgrid::s4u::Engine::get_instance()->get_clock()*1000000));
    (*it)->get()->Log({{"end",t2.count()}});
    (*it)->get()->Finish({opentracing::v3::FinishTimestamp(t2)});
    (*it)->reset();
    (*it)=NULL;
        }
  }
      // TODO DELETE
    }

break;
	}
}
    void return_post_storage_service(TaskDescription* td) {
      XBT_DEBUG("Return function of service post_storage_service");
      switch (td->requestType)
      {
case RequestType::COMPOSE_1:
      if(td->nbHops == 2){
        XBT_DEBUG("Output Function for COMPOSE_1, put to test");
        s4u_Mailbox* m_user_timeline = s4u_Mailbox::by_name("test");
        m_user_timeline->put(td, td->dSize);
      }

break;
	}
}
/* PR FUNCTIONS, AUTO GENERATED CODE, MODIFY IF YOU KNOW WHAT YOU WANT */
    double pr_nginx_web_server(TaskDescription* td) {
      XBT_DEBUG("pr function of service nginx_web_server");
      switch (td->requestType)
      {
case RequestType::COMPOSE:
      if(td->nbHops == 1){XBT_DEBUG("Entered cost Function for nginx_web_server"); return 1564000;
}
break;

      	}//it should never end up here
return -1;
}
    double pr_home_timeline_service(TaskDescription* td) {
      XBT_DEBUG("pr function of service home_timeline_service");
      switch (td->requestType)
      {
case RequestType::COMPOSE_0:
      if(td->nbHops == 2){XBT_DEBUG("Entered cost Function for home_timeline_service"); return 1142000;
}
break;

      	}//it should never end up here
return -1;
}
    double pr_test(TaskDescription* td) {
      XBT_DEBUG("pr function of service test");
      switch (td->requestType)
      {

case RequestType::COMPOSE_0_0:
    if(td->nbHops == 3){XBT_DEBUG("Entered cost Function for test"); return 150000;}

break;


case RequestType::COMPOSE_1:
      if(td->nbHops == 3){XBT_DEBUG("Entered cost Function for COMPOSE_1"); return 50000;}

break;
	}//it should never end up here
return -1;
}
    double pr_test3(TaskDescription* td) {
      XBT_DEBUG("pr function of service test3");
      switch (td->requestType)
      {

case RequestType::COMPOSE_0_1:
    if(td->nbHops == 3){XBT_DEBUG("Entered cost Function for test3"); return 100000;}

break;
	}//it should never end up here
return -1;
}
    double pr_post_storage_service(TaskDescription* td) {
      XBT_DEBUG("pr function of service post_storage_service");
      switch (td->requestType)
      {
case RequestType::COMPOSE_1:
      if(td->nbHops == 2){XBT_DEBUG("Entered cost Function for COMPOSE_1"); return 17000;}

break;
	}//it should never end up here
return -1;
}
  void run() {
    XBT_INFO("Starting run()");

    // create ETM for service nginx_web_server
    std::vector<std::string> v_serv_nginx_web_server = std::vector<std::string>();
    v_serv_nginx_web_server.push_back("nginx_web_server");
    std::shared_ptr<simgrid::s4u::ElasticTaskManager> serv_nginx_web_server = std::make_shared<simgrid::s4u::ElasticTaskManager>("nginx_web_server",v_serv_nginx_web_server);
    serv_nginx_web_server->setBootDuration(0);
    serv_nginx_web_server->setDataSizeRatio(1);
    serv_nginx_web_server->setOutputFunction(return_nginx_web_server);
    serv_nginx_web_server->setProcessRatioFunc(pr_nginx_web_server);
    serv_nginx_web_server->addHost(simgrid::s4u::Host::by_name("cb1-1"));
    simgrid::s4u::Actor::create("etm_nginx_web_server", simgrid::s4u::Host::by_name("cb1-1"), [serv_nginx_web_server] { serv_nginx_web_server->run(); });


    // create ETM for service home_timeline_service
    std::vector<std::string> v_serv_home_timeline_service = std::vector<std::string>();
    v_serv_home_timeline_service.push_back("home_timeline_service");
    std::shared_ptr<simgrid::s4u::ElasticTaskManager> serv_home_timeline_service = std::make_shared<simgrid::s4u::ElasticTaskManager>("home_timeline_service",v_serv_home_timeline_service);
    serv_home_timeline_service->setBootDuration(0);
    serv_home_timeline_service->setDataSizeRatio(1);
    serv_home_timeline_service->setOutputFunction(return_home_timeline_service);
    serv_home_timeline_service->setProcessRatioFunc(pr_home_timeline_service);
    serv_home_timeline_service->addHost(simgrid::s4u::Host::by_name("cb1-2"));
    simgrid::s4u::Actor::create("etm_home_timeline_service", simgrid::s4u::Host::by_name("cb1-2"), [serv_home_timeline_service] { serv_home_timeline_service->run(); });


    // create ETM for service test
    std::vector<std::string> v_serv_test = std::vector<std::string>();
    v_serv_test.push_back("test");
    std::shared_ptr<simgrid::s4u::ElasticTaskManager> serv_test = std::make_shared<simgrid::s4u::ElasticTaskManager>("test",v_serv_test);
    serv_test->setBootDuration(0);
    serv_test->setDataSizeRatio(1);
    serv_test->setOutputFunction(return_test);
    serv_test->setProcessRatioFunc(pr_test);
    serv_test->addHost(simgrid::s4u::Host::by_name("cb1-3"));
    simgrid::s4u::Actor::create("etm_test", simgrid::s4u::Host::by_name("cb1-3"), [serv_test] { serv_test->run(); });


    // create ETM for service test3
    std::vector<std::string> v_serv_test3 = std::vector<std::string>();
    v_serv_test3.push_back("test3");
    std::shared_ptr<simgrid::s4u::ElasticTaskManager> serv_test3 = std::make_shared<simgrid::s4u::ElasticTaskManager>("test3",v_serv_test3);
    serv_test3->setBootDuration(0);
    serv_test3->setDataSizeRatio(1);
    serv_test3->setOutputFunction(return_test3);
    serv_test3->setProcessRatioFunc(pr_test3);
    serv_test3->addHost(simgrid::s4u::Host::by_name("cb1-4"));
    simgrid::s4u::Actor::create("etm_test3", simgrid::s4u::Host::by_name("cb1-4"), [serv_test3] { serv_test3->run(); });


    // create ETM for service post_storage_service
    std::vector<std::string> v_serv_post_storage_service = std::vector<std::string>();
    v_serv_post_storage_service.push_back("post_storage_service");
    std::shared_ptr<simgrid::s4u::ElasticTaskManager> serv_post_storage_service = std::make_shared<simgrid::s4u::ElasticTaskManager>("post_storage_service",v_serv_post_storage_service);
    serv_post_storage_service->setBootDuration(0);
    serv_post_storage_service->setDataSizeRatio(1);
    serv_post_storage_service->setOutputFunction(return_post_storage_service);
    serv_post_storage_service->setProcessRatioFunc(pr_post_storage_service);
    serv_post_storage_service->addHost(simgrid::s4u::Host::by_name("cb1-5"));
    simgrid::s4u::Actor::create("etm_post_storage_service", simgrid::s4u::Host::by_name("cb1-5"), [serv_post_storage_service] { serv_post_storage_service->run(); });



  /* ADD DATASOURCES MANUALLY HERE, SET THE END TIMER AS YOU WISH, AND LAUNCH YOUR SIMULATOR*/
  DataSourceFixedInterval* dsf = new DataSourceFixedInterval("nginx_web_server",RequestType::COMPOSE, 100,100);
  simgrid::s4u::ActorPtr dataS = simgrid::s4u::Actor::create("snd", simgrid::s4u::Host::by_name("cb1-100"), [&]{dsf->run();});

  // kill policies and ETMs
  simgrid::s4u::this_actor::sleep_for(150); /*set it according to your needs*/
  XBT_INFO("Done. Killing policies and etms");
    dataS->kill();

    serv_nginx_web_server->kill();
    serv_home_timeline_service->kill();
    serv_test->kill();
    serv_test3->kill();
    serv_post_storage_service->kill();
}
int main(int argc, char* argv[]) {
	simgrid::s4u::Engine* e = new simgrid::s4u::Engine(&argc, argv);

	e->load_platform(argv[1]);
	simgrid::s4u::Actor::create("main", simgrid::s4u::Host::by_name("cb1-200"), [&]{run();});
	e->run();
	return 0;
}
