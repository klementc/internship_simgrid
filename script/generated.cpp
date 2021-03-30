
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
      
          XBT_DEBUG("Put to media_service");
          s4u_Mailbox* m_media_service = s4u_Mailbox::by_name("media_service");
          TaskDescription* td_0 = new TaskDescription(*td);
          td_0->requestType = RequestType::COMPOSE_0;
          m_media_service->put(td_0, td_0->dSize);
        
          XBT_DEBUG("Put to user_service");
          s4u_Mailbox* m_user_service = s4u_Mailbox::by_name("user_service");
          TaskDescription* td_1 = new TaskDescription(*td);
          td_1->requestType = RequestType::COMPOSE_1;
          m_user_service->put(td_1, td_1->dSize);
        
          XBT_DEBUG("Put to text_service");
          s4u_Mailbox* m_text_service = s4u_Mailbox::by_name("text_service");
          TaskDescription* td_2 = new TaskDescription(*td);
          td_2->requestType = RequestType::COMPOSE_2;
          m_text_service->put(td_2, td_2->dSize);
        
          XBT_DEBUG("Put to unique_id_service");
          s4u_Mailbox* m_unique_id_service = s4u_Mailbox::by_name("unique_id_service");
          TaskDescription* td_3 = new TaskDescription(*td);
          td_3->requestType = RequestType::COMPOSE_3;
          m_unique_id_service->put(td_3, td_3->dSize);
        
}
break;
	}
}
    void return_media_service(TaskDescription* td) {
      XBT_DEBUG("Return function of service media_service");
      switch (td->requestType)
      {
case RequestType::COMPOSE_0:
      if(td->nbHops == 2){
        XBT_DEBUG("Output Function for COMPOSE_0, put to compose_post_service");
        s4u_Mailbox* m_user_timeline = s4u_Mailbox::by_name("compose_post_service");
        m_user_timeline->put(td, td->dSize);
      }
      
break;
	}
}
    void return_compose_post_service(TaskDescription* td) {
      XBT_DEBUG("Return function of service compose_post_service");
      switch (td->requestType)
      {
case RequestType::COMPOSE_0:
      if(td->nbHops == 3){
        XBT_DEBUG("Output Function for COMPOSE_0, Final service, DELETE");
        delete td;
        // TODO DELETE JAEGER SPANS
      }
      
break;


case RequestType::COMPOSE_1:
      if(td->nbHops == 3){
        XBT_DEBUG("Output Function for COMPOSE_1, Final service, DELETE");
        delete td;
        // TODO DELETE JAEGER SPANS
      }
      
break;


case RequestType::COMPOSE_2_0:
      if(td->nbHops == 4){
        XBT_DEBUG("Output Function for COMPOSE_2_0, Final service, DELETE");
        delete td;
        // TODO DELETE JAEGER SPANS
      }
      
break;


case RequestType::COMPOSE_2_1:
      if(td->nbHops == 4){
        XBT_DEBUG("Output Function for COMPOSE_2_1, Final service, DELETE");
        delete td;
        // TODO DELETE JAEGER SPANS
      }
      
break;


case RequestType::COMPOSE_2_2:
      if(td->nbHops == 3){
        XBT_DEBUG("Output Function for COMPOSE_2_2, put to post_storage_service");
        s4u_Mailbox* m_user_timeline = s4u_Mailbox::by_name("post_storage_service");
        m_user_timeline->put(td, td->dSize);
      }
      
      if(td->nbHops == 5){
        XBT_DEBUG("Output Function for COMPOSE_2_2, put to user_timeline_service");
        s4u_Mailbox* m_user_timeline = s4u_Mailbox::by_name("user_timeline_service");
        m_user_timeline->put(td, td->dSize);
      }
      
      if(td->nbHops == 7){
        XBT_DEBUG("Output Function for COMPOSE_2_2, put to write_home_timeline_service");
        s4u_Mailbox* m_user_timeline = s4u_Mailbox::by_name("write_home_timeline_service");
        m_user_timeline->put(td, td->dSize);
      }
      
break;


case RequestType::COMPOSE_3:
      if(td->nbHops == 3){
        XBT_DEBUG("Output Function for COMPOSE_3, Final service, DELETE");
        delete td;
        // TODO DELETE JAEGER SPANS
      }
      
break;
	}
}
    void return_user_service(TaskDescription* td) {
      XBT_DEBUG("Return function of service user_service");
      switch (td->requestType)
      {
case RequestType::COMPOSE_1:
      if(td->nbHops == 2){
        XBT_DEBUG("Output Function for COMPOSE_1, put to compose_post_service");
        s4u_Mailbox* m_user_timeline = s4u_Mailbox::by_name("compose_post_service");
        m_user_timeline->put(td, td->dSize);
      }
      
break;
	}
}
    void return_text_service(TaskDescription* td) {
      XBT_DEBUG("Return function of service text_service");
      switch (td->requestType)
      {
case RequestType::COMPOSE_2:
      if(td->nbHops == 2){
        XBT_DEBUG("Output Function for text_service (PARALLEL CHILDS)");
      
          XBT_DEBUG("Put to url_shorten_service");
          s4u_Mailbox* m_url_shorten_service = s4u_Mailbox::by_name("url_shorten_service");
          TaskDescription* td_0 = new TaskDescription(*td);
          td_0->requestType = RequestType::COMPOSE_2_0;
          m_url_shorten_service->put(td_0, td_0->dSize);
        
          XBT_DEBUG("Put to user_mention_service");
          s4u_Mailbox* m_user_mention_service = s4u_Mailbox::by_name("user_mention_service");
          TaskDescription* td_1 = new TaskDescription(*td);
          td_1->requestType = RequestType::COMPOSE_2_1;
          m_user_mention_service->put(td_1, td_1->dSize);
        
          XBT_DEBUG("Put to compose_post_service");
          s4u_Mailbox* m_compose_post_service = s4u_Mailbox::by_name("compose_post_service");
          TaskDescription* td_2 = new TaskDescription(*td);
          td_2->requestType = RequestType::COMPOSE_2_2;
          m_compose_post_service->put(td_2, td_2->dSize);
        
}
break;
	}
}
    void return_url_shorten_service(TaskDescription* td) {
      XBT_DEBUG("Return function of service url_shorten_service");
      switch (td->requestType)
      {
case RequestType::COMPOSE_2_0:
      if(td->nbHops == 3){
        XBT_DEBUG("Output Function for COMPOSE_2_0, put to compose_post_service");
        s4u_Mailbox* m_user_timeline = s4u_Mailbox::by_name("compose_post_service");
        m_user_timeline->put(td, td->dSize);
      }
      
break;
	}
}
    void return_user_mention_service(TaskDescription* td) {
      XBT_DEBUG("Return function of service user_mention_service");
      switch (td->requestType)
      {
case RequestType::COMPOSE_2_1:
      if(td->nbHops == 3){
        XBT_DEBUG("Output Function for COMPOSE_2_1, put to compose_post_service");
        s4u_Mailbox* m_user_timeline = s4u_Mailbox::by_name("compose_post_service");
        m_user_timeline->put(td, td->dSize);
      }
      
break;
	}
}
    void return_post_storage_service(TaskDescription* td) {
      XBT_DEBUG("Return function of service post_storage_service");
      switch (td->requestType)
      {
case RequestType::COMPOSE_2_2:
      if(td->nbHops == 4){
        XBT_DEBUG("Output Function for COMPOSE_2_2, put to compose_post_service");
        s4u_Mailbox* m_user_timeline = s4u_Mailbox::by_name("compose_post_service");
        m_user_timeline->put(td, td->dSize);
      }
      
break;
	}
}
    void return_user_timeline_service(TaskDescription* td) {
      XBT_DEBUG("Return function of service user_timeline_service");
      switch (td->requestType)
      {
case RequestType::COMPOSE_2_2:
      if(td->nbHops == 6){
        XBT_DEBUG("Output Function for COMPOSE_2_2, put to compose_post_service");
        s4u_Mailbox* m_user_timeline = s4u_Mailbox::by_name("compose_post_service");
        m_user_timeline->put(td, td->dSize);
      }
      
break;
	}
}
    void return_write_home_timeline_service(TaskDescription* td) {
      XBT_DEBUG("Return function of service write_home_timeline_service");
      switch (td->requestType)
      {
case RequestType::COMPOSE_2_2:
      if(td->nbHops == 8){
        XBT_DEBUG("Output Function for COMPOSE_2_2, put to social_graph_service");
        s4u_Mailbox* m_user_timeline = s4u_Mailbox::by_name("social_graph_service");
        m_user_timeline->put(td, td->dSize);
      }
      
      if(td->nbHops == 10){
        XBT_DEBUG("Output Function for COMPOSE_2_2, Final service, DELETE");
        delete td;
        // TODO DELETE JAEGER SPANS
      }
      
break;
	}
}
    void return_social_graph_service(TaskDescription* td) {
      XBT_DEBUG("Return function of service social_graph_service");
      switch (td->requestType)
      {
case RequestType::COMPOSE_2_2:
      if(td->nbHops == 9){
        XBT_DEBUG("Output Function for COMPOSE_2_2, put to write_home_timeline_service");
        s4u_Mailbox* m_user_timeline = s4u_Mailbox::by_name("write_home_timeline_service");
        m_user_timeline->put(td, td->dSize);
      }
      
break;
	}
}
    void return_unique_id_service(TaskDescription* td) {
      XBT_DEBUG("Return function of service unique_id_service");
      switch (td->requestType)
      {
case RequestType::COMPOSE_3:
      if(td->nbHops == 2){
        XBT_DEBUG("Output Function for COMPOSE_3, put to compose_post_service");
        s4u_Mailbox* m_user_timeline = s4u_Mailbox::by_name("compose_post_service");
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
      if(td->nbHops == 1){XBT_DEBUG("Entered cost Function for nginx_web_server"); return 1876000;
}
break;

      	}//it should never end up here
return -1;
}
    double pr_media_service(TaskDescription* td) {
      XBT_DEBUG("pr function of service media_service");
      switch (td->requestType)
      {
case RequestType::COMPOSE_0:
      if(td->nbHops == 2){XBT_DEBUG("Entered cost Function for COMPOSE_0"); return 415000;}
      
break;
	}//it should never end up here
return -1;
}
    double pr_compose_post_service(TaskDescription* td) {
      XBT_DEBUG("pr function of service compose_post_service");
      switch (td->requestType)
      {
case RequestType::COMPOSE_0:
      if(td->nbHops == 3){XBT_DEBUG("Entered cost Function for COMPOSE_0 (FINAL NODE, NO CHILD!)"); return 770000;}
      
break;


case RequestType::COMPOSE_1:
      if(td->nbHops == 3){XBT_DEBUG("Entered cost Function for COMPOSE_1 (FINAL NODE, NO CHILD!)"); return 579000;}
      
break;


case RequestType::COMPOSE_2_0:
      if(td->nbHops == 4){XBT_DEBUG("Entered cost Function for COMPOSE_2_0 (FINAL NODE, NO CHILD!)"); return 652000;}
      
break;


case RequestType::COMPOSE_2_1:
      if(td->nbHops == 4){XBT_DEBUG("Entered cost Function for COMPOSE_2_1 (FINAL NODE, NO CHILD!)"); return 938000;}
      
break;


case RequestType::COMPOSE_2_2:
      if(td->nbHops == 3){XBT_DEBUG("Entered cost Function for COMPOSE_2_2"); return 1860000;}
      
      if(td->nbHops == 5){XBT_DEBUG("Entered cost Function for COMPOSE_2_2"); return 0;}
      
      if(td->nbHops == 7){XBT_DEBUG("Entered cost Function for COMPOSE_2_2"); return 0;}
      
break;


case RequestType::COMPOSE_3:
      if(td->nbHops == 3){XBT_DEBUG("Entered cost Function for COMPOSE_3 (FINAL NODE, NO CHILD!)"); return 629000;}
      
break;
	}//it should never end up here
return -1;
}
    double pr_user_service(TaskDescription* td) {
      XBT_DEBUG("pr function of service user_service");
      switch (td->requestType)
      {
case RequestType::COMPOSE_1:
      if(td->nbHops == 2){XBT_DEBUG("Entered cost Function for COMPOSE_1"); return 334000;}
      
break;
	}//it should never end up here
return -1;
}
    double pr_text_service(TaskDescription* td) {
      XBT_DEBUG("pr function of service text_service");
      switch (td->requestType)
      {
case RequestType::COMPOSE_2:
      if(td->nbHops == 2){XBT_DEBUG("Entered cost Function for text_service"); return 1578000;
}
break;

      	}//it should never end up here
return -1;
}
    double pr_url_shorten_service(TaskDescription* td) {
      XBT_DEBUG("pr function of service url_shorten_service");
      switch (td->requestType)
      {
case RequestType::COMPOSE_2_0:
      if(td->nbHops == 3){XBT_DEBUG("Entered cost Function for COMPOSE_2_0"); return 837000;}
      
break;
	}//it should never end up here
return -1;
}
    double pr_user_mention_service(TaskDescription* td) {
      XBT_DEBUG("pr function of service user_mention_service");
      switch (td->requestType)
      {
case RequestType::COMPOSE_2_1:
      if(td->nbHops == 3){XBT_DEBUG("Entered cost Function for COMPOSE_2_1"); return 1934000;}
      
break;
	}//it should never end up here
return -1;
}
    double pr_post_storage_service(TaskDescription* td) {
      XBT_DEBUG("pr function of service post_storage_service");
      switch (td->requestType)
      {
case RequestType::COMPOSE_2_2:
      if(td->nbHops == 4){XBT_DEBUG("Entered cost Function for COMPOSE_2_2"); return 976000;}
      
break;
	}//it should never end up here
return -1;
}
    double pr_user_timeline_service(TaskDescription* td) {
      XBT_DEBUG("pr function of service user_timeline_service");
      switch (td->requestType)
      {
case RequestType::COMPOSE_2_2:
      if(td->nbHops == 6){XBT_DEBUG("Entered cost Function for COMPOSE_2_2"); return 3950000;}
      
break;
	}//it should never end up here
return -1;
}
    double pr_write_home_timeline_service(TaskDescription* td) {
      XBT_DEBUG("pr function of service write_home_timeline_service");
      switch (td->requestType)
      {
case RequestType::COMPOSE_2_2:
      if(td->nbHops == 8){XBT_DEBUG("Entered cost Function for COMPOSE_2_2"); return 447000;}
      
      if(td->nbHops == 10){XBT_DEBUG("Entered cost Function for COMPOSE_2_2 (FINAL NODE, NO CHILD!)"); return 47000;}
      
break;
	}//it should never end up here
return -1;
}
    double pr_social_graph_service(TaskDescription* td) {
      XBT_DEBUG("pr function of service social_graph_service");
      switch (td->requestType)
      {
case RequestType::COMPOSE_2_2:
      if(td->nbHops == 9){XBT_DEBUG("Entered cost Function for COMPOSE_2_2"); return 1756000;}
      
break;
	}//it should never end up here
return -1;
}
    double pr_unique_id_service(TaskDescription* td) {
      XBT_DEBUG("pr function of service unique_id_service");
      switch (td->requestType)
      {
case RequestType::COMPOSE_3:
      if(td->nbHops == 2){XBT_DEBUG("Entered cost Function for COMPOSE_3"); return 377000;}
      
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

    
    // create ETM for service media_service
    std::vector<std::string> v_serv_media_service = std::vector<std::string>();
    v_serv_media_service.push_back("media_service");
    std::shared_ptr<simgrid::s4u::ElasticTaskManager> serv_media_service = std::make_shared<simgrid::s4u::ElasticTaskManager>("media_service",v_serv_media_service);
    serv_media_service->setBootDuration(0);
    serv_media_service->setDataSizeRatio(1);
    serv_media_service->setOutputFunction(return_media_service);
    serv_media_service->setProcessRatioFunc(pr_media_service);
    serv_media_service->addHost(simgrid::s4u::Host::by_name("cb1-2"));
    simgrid::s4u::Actor::create("etm_media_service", simgrid::s4u::Host::by_name("cb1-2"), [serv_media_service] { serv_media_service->run(); });

    
    // create ETM for service compose_post_service
    std::vector<std::string> v_serv_compose_post_service = std::vector<std::string>();
    v_serv_compose_post_service.push_back("compose_post_service");
    std::shared_ptr<simgrid::s4u::ElasticTaskManager> serv_compose_post_service = std::make_shared<simgrid::s4u::ElasticTaskManager>("compose_post_service",v_serv_compose_post_service);
    serv_compose_post_service->setBootDuration(0);
    serv_compose_post_service->setDataSizeRatio(1);
    serv_compose_post_service->setOutputFunction(return_compose_post_service);
    serv_compose_post_service->setProcessRatioFunc(pr_compose_post_service);
    serv_compose_post_service->addHost(simgrid::s4u::Host::by_name("cb1-3"));
    simgrid::s4u::Actor::create("etm_compose_post_service", simgrid::s4u::Host::by_name("cb1-3"), [serv_compose_post_service] { serv_compose_post_service->run(); });

    
    // create ETM for service user_service
    std::vector<std::string> v_serv_user_service = std::vector<std::string>();
    v_serv_user_service.push_back("user_service");
    std::shared_ptr<simgrid::s4u::ElasticTaskManager> serv_user_service = std::make_shared<simgrid::s4u::ElasticTaskManager>("user_service",v_serv_user_service);
    serv_user_service->setBootDuration(0);
    serv_user_service->setDataSizeRatio(1);
    serv_user_service->setOutputFunction(return_user_service);
    serv_user_service->setProcessRatioFunc(pr_user_service);
    serv_user_service->addHost(simgrid::s4u::Host::by_name("cb1-4"));
    simgrid::s4u::Actor::create("etm_user_service", simgrid::s4u::Host::by_name("cb1-4"), [serv_user_service] { serv_user_service->run(); });

    
    // create ETM for service text_service
    std::vector<std::string> v_serv_text_service = std::vector<std::string>();
    v_serv_text_service.push_back("text_service");
    std::shared_ptr<simgrid::s4u::ElasticTaskManager> serv_text_service = std::make_shared<simgrid::s4u::ElasticTaskManager>("text_service",v_serv_text_service);
    serv_text_service->setBootDuration(0);
    serv_text_service->setDataSizeRatio(1);
    serv_text_service->setOutputFunction(return_text_service);
    serv_text_service->setProcessRatioFunc(pr_text_service);
    serv_text_service->addHost(simgrid::s4u::Host::by_name("cb1-5"));
    simgrid::s4u::Actor::create("etm_text_service", simgrid::s4u::Host::by_name("cb1-5"), [serv_text_service] { serv_text_service->run(); });

    
    // create ETM for service url_shorten_service
    std::vector<std::string> v_serv_url_shorten_service = std::vector<std::string>();
    v_serv_url_shorten_service.push_back("url_shorten_service");
    std::shared_ptr<simgrid::s4u::ElasticTaskManager> serv_url_shorten_service = std::make_shared<simgrid::s4u::ElasticTaskManager>("url_shorten_service",v_serv_url_shorten_service);
    serv_url_shorten_service->setBootDuration(0);
    serv_url_shorten_service->setDataSizeRatio(1);
    serv_url_shorten_service->setOutputFunction(return_url_shorten_service);
    serv_url_shorten_service->setProcessRatioFunc(pr_url_shorten_service);
    serv_url_shorten_service->addHost(simgrid::s4u::Host::by_name("cb1-6"));
    simgrid::s4u::Actor::create("etm_url_shorten_service", simgrid::s4u::Host::by_name("cb1-6"), [serv_url_shorten_service] { serv_url_shorten_service->run(); });

    
    // create ETM for service user_mention_service
    std::vector<std::string> v_serv_user_mention_service = std::vector<std::string>();
    v_serv_user_mention_service.push_back("user_mention_service");
    std::shared_ptr<simgrid::s4u::ElasticTaskManager> serv_user_mention_service = std::make_shared<simgrid::s4u::ElasticTaskManager>("user_mention_service",v_serv_user_mention_service);
    serv_user_mention_service->setBootDuration(0);
    serv_user_mention_service->setDataSizeRatio(1);
    serv_user_mention_service->setOutputFunction(return_user_mention_service);
    serv_user_mention_service->setProcessRatioFunc(pr_user_mention_service);
    serv_user_mention_service->addHost(simgrid::s4u::Host::by_name("cb1-7"));
    simgrid::s4u::Actor::create("etm_user_mention_service", simgrid::s4u::Host::by_name("cb1-7"), [serv_user_mention_service] { serv_user_mention_service->run(); });

    
    // create ETM for service post_storage_service
    std::vector<std::string> v_serv_post_storage_service = std::vector<std::string>();
    v_serv_post_storage_service.push_back("post_storage_service");
    std::shared_ptr<simgrid::s4u::ElasticTaskManager> serv_post_storage_service = std::make_shared<simgrid::s4u::ElasticTaskManager>("post_storage_service",v_serv_post_storage_service);
    serv_post_storage_service->setBootDuration(0);
    serv_post_storage_service->setDataSizeRatio(1);
    serv_post_storage_service->setOutputFunction(return_post_storage_service);
    serv_post_storage_service->setProcessRatioFunc(pr_post_storage_service);
    serv_post_storage_service->addHost(simgrid::s4u::Host::by_name("cb1-8"));
    simgrid::s4u::Actor::create("etm_post_storage_service", simgrid::s4u::Host::by_name("cb1-8"), [serv_post_storage_service] { serv_post_storage_service->run(); });

    
    // create ETM for service user_timeline_service
    std::vector<std::string> v_serv_user_timeline_service = std::vector<std::string>();
    v_serv_user_timeline_service.push_back("user_timeline_service");
    std::shared_ptr<simgrid::s4u::ElasticTaskManager> serv_user_timeline_service = std::make_shared<simgrid::s4u::ElasticTaskManager>("user_timeline_service",v_serv_user_timeline_service);
    serv_user_timeline_service->setBootDuration(0);
    serv_user_timeline_service->setDataSizeRatio(1);
    serv_user_timeline_service->setOutputFunction(return_user_timeline_service);
    serv_user_timeline_service->setProcessRatioFunc(pr_user_timeline_service);
    serv_user_timeline_service->addHost(simgrid::s4u::Host::by_name("cb1-9"));
    simgrid::s4u::Actor::create("etm_user_timeline_service", simgrid::s4u::Host::by_name("cb1-9"), [serv_user_timeline_service] { serv_user_timeline_service->run(); });

    
    // create ETM for service write_home_timeline_service
    std::vector<std::string> v_serv_write_home_timeline_service = std::vector<std::string>();
    v_serv_write_home_timeline_service.push_back("write_home_timeline_service");
    std::shared_ptr<simgrid::s4u::ElasticTaskManager> serv_write_home_timeline_service = std::make_shared<simgrid::s4u::ElasticTaskManager>("write_home_timeline_service",v_serv_write_home_timeline_service);
    serv_write_home_timeline_service->setBootDuration(0);
    serv_write_home_timeline_service->setDataSizeRatio(1);
    serv_write_home_timeline_service->setOutputFunction(return_write_home_timeline_service);
    serv_write_home_timeline_service->setProcessRatioFunc(pr_write_home_timeline_service);
    serv_write_home_timeline_service->addHost(simgrid::s4u::Host::by_name("cb1-10"));
    simgrid::s4u::Actor::create("etm_write_home_timeline_service", simgrid::s4u::Host::by_name("cb1-10"), [serv_write_home_timeline_service] { serv_write_home_timeline_service->run(); });

    
    // create ETM for service social_graph_service
    std::vector<std::string> v_serv_social_graph_service = std::vector<std::string>();
    v_serv_social_graph_service.push_back("social_graph_service");
    std::shared_ptr<simgrid::s4u::ElasticTaskManager> serv_social_graph_service = std::make_shared<simgrid::s4u::ElasticTaskManager>("social_graph_service",v_serv_social_graph_service);
    serv_social_graph_service->setBootDuration(0);
    serv_social_graph_service->setDataSizeRatio(1);
    serv_social_graph_service->setOutputFunction(return_social_graph_service);
    serv_social_graph_service->setProcessRatioFunc(pr_social_graph_service);
    serv_social_graph_service->addHost(simgrid::s4u::Host::by_name("cb1-11"));
    simgrid::s4u::Actor::create("etm_social_graph_service", simgrid::s4u::Host::by_name("cb1-11"), [serv_social_graph_service] { serv_social_graph_service->run(); });

    
    // create ETM for service unique_id_service
    std::vector<std::string> v_serv_unique_id_service = std::vector<std::string>();
    v_serv_unique_id_service.push_back("unique_id_service");
    std::shared_ptr<simgrid::s4u::ElasticTaskManager> serv_unique_id_service = std::make_shared<simgrid::s4u::ElasticTaskManager>("unique_id_service",v_serv_unique_id_service);
    serv_unique_id_service->setBootDuration(0);
    serv_unique_id_service->setDataSizeRatio(1);
    serv_unique_id_service->setOutputFunction(return_unique_id_service);
    serv_unique_id_service->setProcessRatioFunc(pr_unique_id_service);
    serv_unique_id_service->addHost(simgrid::s4u::Host::by_name("cb1-12"));
    simgrid::s4u::Actor::create("etm_unique_id_service", simgrid::s4u::Host::by_name("cb1-12"), [serv_unique_id_service] { serv_unique_id_service->run(); });

    

  /* ADD DATASOURCES MANUALLY HERE, SET THE END TIMER AS YOU WISH, AND LAUNCH YOUR SIMULATOR*/

  // kill policies and ETMs
  simgrid::s4u::this_actor::sleep_for(150); /*set it according to your needs*/
  XBT_INFO("Done. Killing policies and etms");
  
    serv_nginx_web_server->kill();
    serv_media_service->kill();
    serv_compose_post_service->kill();
    serv_user_service->kill();
    serv_text_service->kill();
    serv_url_shorten_service->kill();
    serv_user_mention_service->kill();
    serv_post_storage_service->kill();
    serv_user_timeline_service->kill();
    serv_write_home_timeline_service->kill();
    serv_social_graph_service->kill();
    serv_unique_id_service->kill();
}
int main(int argc, char* argv[]) {
	simgrid::s4u::Engine* e = new simgrid::s4u::Engine(&argc, argv);

	e->load_platform(argv[1]);
	simgrid::s4u::Actor::create("main", simgrid::s4u::Host::by_name("cb1-200"), [&]{run();});
	e->run();
	return 0;
}
