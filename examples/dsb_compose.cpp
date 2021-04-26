
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

int outSizes(RequestType, std::string);
/* RETURN FUNCTIONS, AUTO GENERATED CODE, MODIFY IF YOU KNOW WHAT YOU WANT */
    void return_nginx_web_server(TaskDescription* td) {
      XBT_DEBUG("Return function of service nginx_web_server");
      switch (td->requestType)
      {
case RequestType::COMPOSE:
      if(td->nbHops == 1){
        XBT_DEBUG("Output Function for COMPOSE, put to compose_post_service");
        s4u_Mailbox* m_compose_post_service = s4u_Mailbox::by_name("compose_post_service");
        m_compose_post_service->put(td, outSizes(td->requestType, "compose_post_service"));


break;
}
	}
}
    void return_compose_post_service(TaskDescription* td) {
      XBT_DEBUG("Return function of service compose_post_service");
      switch (td->requestType)
      {
case RequestType::COMPOSE:
      if(td->nbHops == 2){
        XBT_DEBUG("Output Function for COMPOSE, put to unique_id_service");
        s4u_Mailbox* m_unique_id_service = s4u_Mailbox::by_name("unique_id_service");
        m_unique_id_service->put(td, outSizes(td->requestType, "unique_id_service"));


break;
}

      if(td->nbHops == 4){
        XBT_DEBUG("Output Function for COMPOSE, put to media_service");
        s4u_Mailbox* m_media_service = s4u_Mailbox::by_name("media_service");
        m_media_service->put(td, outSizes(td->requestType, "media_service"));


break;
}

      if(td->nbHops == 6){
        XBT_DEBUG("Output Function for COMPOSE, put to user_service");
        s4u_Mailbox* m_user_service = s4u_Mailbox::by_name("user_service");
        m_user_service->put(td, outSizes(td->requestType, "user_service"));


break;
}

      if(td->nbHops == 8){
        XBT_DEBUG("Output Function for COMPOSE, put to text_service");
        s4u_Mailbox* m_text_service = s4u_Mailbox::by_name("text_service");
        m_text_service->put(td, outSizes(td->requestType, "text_service"));


break;
}


case RequestType::COMPOSE_0:
      if(td->nbHops == 12){
        XBT_DEBUG("Output Function for COMPOSE_0, put to home_timeline_service");
        s4u_Mailbox* m_home_timeline_service = s4u_Mailbox::by_name("home_timeline_service");
        m_home_timeline_service->put(td, outSizes(td->requestType, "home_timeline_service"));


break;
}

      if(td->nbHops == 16){
        XBT_DEBUG("Output Function for COMPOSE_0, put to user_timeline_service");
        s4u_Mailbox* m_user_timeline_service = s4u_Mailbox::by_name("user_timeline_service");
        m_user_timeline_service->put(td, outSizes(td->requestType, "user_timeline_service"));


break;
}

      if(td->nbHops == 18){
        XBT_DEBUG("Output Function for COMPOSE_0, put to post_storage_service");
        s4u_Mailbox* m_post_storage_service = s4u_Mailbox::by_name("post_storage_service");
        m_post_storage_service->put(td, outSizes(td->requestType, "post_storage_service"));


break;
}

break;
	}
}
    void return_unique_id_service(TaskDescription* td) {
      XBT_DEBUG("Return function of service unique_id_service");
      switch (td->requestType)
      {
case RequestType::COMPOSE:
      if(td->nbHops == 3){
        XBT_DEBUG("Output Function for COMPOSE, put to compose_post_service");
        s4u_Mailbox* m_compose_post_service = s4u_Mailbox::by_name("compose_post_service");
        m_compose_post_service->put(td, outSizes(td->requestType, "compose_post_service"));


break;
}
	}
}
    void return_media_service(TaskDescription* td) {
      XBT_DEBUG("Return function of service media_service");
      switch (td->requestType)
      {
case RequestType::COMPOSE:
      if(td->nbHops == 5){
        XBT_DEBUG("Output Function for COMPOSE, put to compose_post_service");
        s4u_Mailbox* m_compose_post_service = s4u_Mailbox::by_name("compose_post_service");
        m_compose_post_service->put(td, outSizes(td->requestType, "compose_post_service"));


break;
}
	}
}
    void return_user_service(TaskDescription* td) {
      XBT_DEBUG("Return function of service user_service");
      switch (td->requestType)
      {
case RequestType::COMPOSE:
      if(td->nbHops == 7){
        XBT_DEBUG("Output Function for COMPOSE, put to compose_post_service");
        s4u_Mailbox* m_compose_post_service = s4u_Mailbox::by_name("compose_post_service");
        m_compose_post_service->put(td, outSizes(td->requestType, "compose_post_service"));


break;
}
	}
}
    void return_text_service(TaskDescription* td) {
      XBT_DEBUG("Return function of service text_service AAAAAAAAAAAAAAAAAAAAA");
      switch (td->requestType)
      {
case RequestType::COMPOSE:
      if(td->nbHops == 9){
        XBT_DEBUG("Output Function for text_service (PARALLEL CHILDS)");
        std::vector<simgrid::s4u::CommPtr> comV;
        simgrid::s4u::CommPtr com;

          XBT_DEBUG("Put to text_service");
          s4u_Mailbox* m_text_service = s4u_Mailbox::by_name("text_service");
          TaskDescription* td_0 = new TaskDescription(*td);
          td_0->requestType = RequestType::COMPOSE_0;
          com = m_text_service->put_async(td_0, outSizes(td_0->requestType, "text_service"));
          comV.push_back(com);

          XBT_DEBUG("Put to text_service");
          s4u_Mailbox* m_text_service_ = s4u_Mailbox::by_name("text_service");
          TaskDescription* td_1 = new TaskDescription(*td);
          td_1->nbHops = 9;
          td_1->requestType = RequestType::COMPOSE_1;
          com = m_text_service_->put_async(td_1, outSizes(td_1->requestType, "text_service"));
          comV.push_back(com);

      simgrid::s4u::Comm::wait_all(&comV); // wait for communications to finish
      break;
      }


case RequestType::COMPOSE_0:
      if(td->nbHops == 10){
        XBT_DEBUG("Output Function for COMPOSE_0, put to user_mention_service");
        s4u_Mailbox* m_user_mention_service = s4u_Mailbox::by_name("user_mention_service");
        m_user_mention_service->put(td, outSizes(td->requestType, "user_mention_service"));


      break;
      }

break;


case RequestType::COMPOSE_1:
      if(td->nbHops == 10){
        XBT_DEBUG("Output Function for COMPOSE_1, put to url_shorten_service");
        s4u_Mailbox* m_url_shorten_service = s4u_Mailbox::by_name("url_shorten_service");
        m_url_shorten_service->put(td, outSizes(td->requestType, "url_shorten_service"));


break;
}

break;
	}
}
    void return_user_mention_service(TaskDescription* td) {
      XBT_DEBUG("Return function of service user_mention_service");
      switch (td->requestType)
      {
case RequestType::COMPOSE_0:
      if(td->nbHops == 11){
        XBT_DEBUG("Output Function for COMPOSE_0, put to compose_post_service");
        s4u_Mailbox* m_compose_post_service = s4u_Mailbox::by_name("compose_post_service");
        m_compose_post_service->put(td, outSizes(td->requestType, "compose_post_service"));


break;
}

break;
	}
}
    void return_home_timeline_service(TaskDescription* td) {
      XBT_DEBUG("Return function of service home_timeline_service");
      switch (td->requestType)
      {
case RequestType::COMPOSE_0:
      if(td->nbHops == 13){
        XBT_DEBUG("Output Function for COMPOSE_0, put to social_graph_service");
        s4u_Mailbox* m_social_graph_service = s4u_Mailbox::by_name("social_graph_service");
        m_social_graph_service->put(td, outSizes(td->requestType, "social_graph_service"));


break;
}

      if(td->nbHops == 15){
        XBT_DEBUG("Output Function for COMPOSE_0, put to compose_post_service");
        s4u_Mailbox* m_compose_post_service = s4u_Mailbox::by_name("compose_post_service");
        m_compose_post_service->put(td, outSizes(td->requestType, "compose_post_service"));


break;
}

break;
	}
}
    void return_social_graph_service(TaskDescription* td) {
      XBT_DEBUG("Return function of service social_graph_service");
      switch (td->requestType)
      {
case RequestType::COMPOSE_0:
      if(td->nbHops == 14){
        XBT_DEBUG("Output Function for COMPOSE_0, put to home_timeline_service");
        s4u_Mailbox* m_home_timeline_service = s4u_Mailbox::by_name("home_timeline_service");
        m_home_timeline_service->put(td, outSizes(td->requestType, "home_timeline_service"));


break;
}

break;
	}
}
    void return_user_timeline_service(TaskDescription* td) {
      XBT_DEBUG("Return function of service user_timeline_service");
      switch (td->requestType)
      {
case RequestType::COMPOSE_0:
      if(td->nbHops == 17){
        XBT_DEBUG("Output Function for COMPOSE_0, put to compose_post_service");
        s4u_Mailbox* m_compose_post_service = s4u_Mailbox::by_name("compose_post_service");
        m_compose_post_service->put(td, outSizes(td->requestType, "compose_post_service"));


break;
}

break;
	}
}
    void return_post_storage_service(TaskDescription* td) {
      XBT_DEBUG("Return function of service post_storage_service");
      switch (td->requestType)
      {
case RequestType::COMPOSE_0:
      if(td->nbHops == 19){
            XBT_INFO("FINISHED REQUEST at ts %lf", simgrid::s4u::Engine::get_clock());
        XBT_DEBUG("Output Function for COMPOSE_0, Final service, DELETE");
        for (auto it = td->parentSpans.rbegin(); it != td->parentSpans.rend(); ++it)
  {
    if((*it)->get() != NULL){
    auto t2 = std::chrono::seconds(946684800)+std::chrono::microseconds(int(simgrid::s4u::Engine::get_instance()->get_clock()*1000000));
    (*it)->get()->Log({{"end",t2.count()}});
    (*it)->get()->Finish({opentracing::v3::FinishTimestamp(t2)});
    (*it)->reset();
    (*it) == NULL;
    }
  }
        delete td;
        // TODO DELETE JAEGER SPANS
      }

break;
	}
}
    void return_url_shorten_service(TaskDescription* td) {
      XBT_DEBUG("Return function of service url_shorten_service");
      switch (td->requestType)
      {
case RequestType::COMPOSE_1:
      if(td->nbHops == 11){
        XBT_DEBUG("Output Function for COMPOSE_1, Final service, DELETE");
        for (auto it = td->parentSpans.rbegin(); it != td->parentSpans.rend(); ++it)
  {
    if((*it)->get() != NULL){
    auto t2 = std::chrono::seconds(946684800)+std::chrono::microseconds(int(simgrid::s4u::Engine::get_instance()->get_clock()*1000000));
    (*it)->get()->Log({{"end",t2.count()}});
    (*it)->get()->Finish({opentracing::v3::FinishTimestamp(t2)});
    (*it)->reset();
    (*it) == NULL;
    }
  }
        delete td;
        // TODO DELETE JAEGER SPANS
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
      if(td->nbHops == 1){XBT_DEBUG("Entered cost Function for COMPOSE"); return 500000;}
      	}//it should never end up here
return -1;
}
    double pr_compose_post_service(TaskDescription* td) {
      XBT_DEBUG("pr function of service compose_post_service");
      switch (td->requestType)
      {
case RequestType::COMPOSE:
      if(td->nbHops == 2){XBT_DEBUG("Entered cost Function for COMPOSE"); return 608000;}

      if(td->nbHops == 4){XBT_DEBUG("Entered cost Function for COMPOSE"); return 118000;}

      if(td->nbHops == 6){XBT_DEBUG("Entered cost Function for COMPOSE"); return 117000;}

      if(td->nbHops == 8){XBT_DEBUG("Entered cost Function for COMPOSE"); return 134000;}


case RequestType::COMPOSE_0:
      if(td->nbHops == 12){XBT_DEBUG("Entered cost Function for COMPOSE_0"); return 264000;}

      if(td->nbHops == 16){XBT_DEBUG("Entered cost Function for COMPOSE_0"); return 173000;}

      if(td->nbHops == 18){XBT_DEBUG("Entered cost Function for COMPOSE_0"); return 288000;}

break;
	}//it should never end up here
return -1;
}
    double pr_unique_id_service(TaskDescription* td) {
      XBT_DEBUG("pr function of service unique_id_service");
      switch (td->requestType)
      {
case RequestType::COMPOSE:
      if(td->nbHops == 3){XBT_DEBUG("Entered cost Function for COMPOSE"); return 11000;}
      	}//it should never end up here
return -1;
}
    double pr_media_service(TaskDescription* td) {
      XBT_DEBUG("pr function of service media_service");
      switch (td->requestType)
      {
case RequestType::COMPOSE:
      if(td->nbHops == 5){XBT_DEBUG("Entered cost Function for COMPOSE"); return 6000;}
      	}//it should never end up here
return -1;
}
    double pr_user_service(TaskDescription* td) {
      XBT_DEBUG("pr function of service user_service");
      switch (td->requestType)
      {
case RequestType::COMPOSE:
      if(td->nbHops == 7){XBT_DEBUG("Entered cost Function for COMPOSE"); return 5000;}
      	}//it should never end up here
return -1;
}
    double pr_text_service(TaskDescription* td) {
      XBT_DEBUG("pr function of service text_service");
      switch (td->requestType)
      {
case RequestType::COMPOSE:
      if(td->nbHops == 9){XBT_DEBUG("Entered cost Function for text_service"); return 318000;
}
break;



case RequestType::COMPOSE_0:
      if(td->nbHops == 10){XBT_DEBUG("Entered cost Function for COMPOSE_0"); return 159000;}

break;


case RequestType::COMPOSE_1:
      if(td->nbHops == 10){XBT_DEBUG("Entered cost Function for COMPOSE_1"); return 457000;}

break;
	}//it should never end up here
return -1;
}
    double pr_user_mention_service(TaskDescription* td) {
      XBT_DEBUG("pr function of service user_mention_service");
      switch (td->requestType)
      {
case RequestType::COMPOSE_0:
      if(td->nbHops == 11){XBT_DEBUG("Entered cost Function for COMPOSE_0"); return 724000;}

break;
	}//it should never end up here
return -1;
}
    double pr_home_timeline_service(TaskDescription* td) {
      XBT_DEBUG("pr function of service home_timeline_service");
      switch (td->requestType)
      {
case RequestType::COMPOSE_0:
      if(td->nbHops == 13){XBT_DEBUG("Entered cost Function for COMPOSE_0"); return 168000;}

      if(td->nbHops == 15){XBT_DEBUG("Entered cost Function for COMPOSE_0"); return 5000;}

break;
	}//it should never end up here
return -1;
}
    double pr_social_graph_service(TaskDescription* td) {
      XBT_DEBUG("pr function of service social_graph_service");
      switch (td->requestType)
      {
case RequestType::COMPOSE_0:
      if(td->nbHops == 14){XBT_DEBUG("Entered cost Function for COMPOSE_0"); return 707000;}

break;
	}//it should never end up here
return -1;
}
    double pr_user_timeline_service(TaskDescription* td) {
      XBT_DEBUG("pr function of service user_timeline_service");
      switch (td->requestType)
      {
case RequestType::COMPOSE_0:
      if(td->nbHops == 17){XBT_DEBUG("Entered cost Function for COMPOSE_0"); return 834000;}

break;
	}//it should never end up here
return -1;
}
    double pr_post_storage_service(TaskDescription* td) {
      XBT_DEBUG("pr function of service post_storage_service");
      switch (td->requestType)
      {
case RequestType::COMPOSE_0:
      if(td->nbHops == 19){XBT_DEBUG("Entered cost Function for COMPOSE_0 (FINAL NODE, NO CHILD!)"); return 508000;}

break;
	}//it should never end up here
return -1;
}
    double pr_url_shorten_service(TaskDescription* td) {
      XBT_DEBUG("pr function of service url_shorten_service");
      switch (td->requestType)
      {
case RequestType::COMPOSE_1:
      if(td->nbHops == 11){XBT_DEBUG("Entered cost Function for COMPOSE_1 (FINAL NODE, NO CHILD!)"); return 451000;}

break;
	}//it should never end up here
return -1;
}

int outSizes(RequestType t, std::string nextServ){
    switch (t)
{	case RequestType::COMPOSE :
	if(nextServ == "compose_post_service"){return 100;}
	if(nextServ == "unique_id_service"){return 100;}
	if(nextServ == "media_service"){return 100;}
	if(nextServ == "user_service"){return 100;}
	if(nextServ == "text_service"){return 100;}
	if(nextServ == "user_mention_service"){return 100;}
	if(nextServ == "home_timeline_service"){return 100;}
	if(nextServ == "social_graph_service"){return 100;}
	if(nextServ == "user_timeline_service"){return 100;}
	if(nextServ == "post_storage_service"){return 100;}
	if(nextServ == "url_shorten_service"){return 100;}
	break;
	case RequestType::COMPOSE_0 :
	if(nextServ == "compose_post_service"){return 100;}
	if(nextServ == "unique_id_service"){return 100;}
	if(nextServ == "media_service"){return 100;}
	if(nextServ == "user_service"){return 100;}
	if(nextServ == "text_service"){return 100;}
	if(nextServ == "user_mention_service"){return 100;}
	if(nextServ == "home_timeline_service"){return 100;}
	if(nextServ == "social_graph_service"){return 100;}
	if(nextServ == "user_timeline_service"){return 100;}
	if(nextServ == "post_storage_service"){return 100;}
	if(nextServ == "url_shorten_service"){return 100;}
	break;
	case RequestType::COMPOSE_1 :
	if(nextServ == "compose_post_service"){return 100;}
	if(nextServ == "unique_id_service"){return 100;}
	if(nextServ == "media_service"){return 100;}
	if(nextServ == "user_service"){return 100;}
	if(nextServ == "text_service"){return 100;}
	if(nextServ == "user_mention_service"){return 100;}
	if(nextServ == "home_timeline_service"){return 100;}
	if(nextServ == "social_graph_service"){return 100;}
	if(nextServ == "user_timeline_service"){return 100;}
	if(nextServ == "post_storage_service"){return 100;}
	if(nextServ == "url_shorten_service"){return 100;}
	break;
}return -1; // WE SHOULD NEVER GET TO THAT POINT
}

std::string reqTypeToStr(RequestType t){
    switch (t)
{	case RequestType::COMPOSE : return "COMPOSE";break;
	case RequestType::COMPOSE_0 : return "COMPOSE_0";break;
	case RequestType::COMPOSE_1 : return "COMPOSE_1";break;
}return "WTF";
}

  void run(std::map<std::string, std::string> m, double freq) {
    XBT_INFO("Starting run()");
    std::string posS;

    // create ETM for service nginx_web_server
    std::vector<std::string> v_serv_nginx_web_server = std::vector<std::string>();
    v_serv_nginx_web_server.push_back("nginx_web_server");
    std::shared_ptr<simgrid::s4u::ElasticTaskManager> serv_nginx_web_server = std::make_shared<simgrid::s4u::ElasticTaskManager>("nginx_web_server",v_serv_nginx_web_server);
    serv_nginx_web_server->setBootDuration(0);
    serv_nginx_web_server->setParallelTasksPerInst(1);
    serv_nginx_web_server->setOutputFunction(return_nginx_web_server);
    serv_nginx_web_server->setExecAmountFunc(pr_nginx_web_server);
    serv_nginx_web_server->setReqNames(reqTypeToStr);
    if (m.find("nginx_web_server") != m.end()) {
        posS = m.find("nginx_web_server")->second;
    }else{
        posS = m.find("default")->second;
    }
    XBT_INFO("nginx_web_server on %s", posS.c_str() );
    serv_nginx_web_server->addHost(simgrid::s4u::Host::by_name(posS));
    simgrid::s4u::Actor::create("etm_nginx_web_server", simgrid::s4u::Host::by_name(posS), [serv_nginx_web_server] { serv_nginx_web_server->run(); });


    // create ETM for service compose_post_service
    std::vector<std::string> v_serv_compose_post_service = std::vector<std::string>();
    v_serv_compose_post_service.push_back("compose_post_service");
    std::shared_ptr<simgrid::s4u::ElasticTaskManager> serv_compose_post_service = std::make_shared<simgrid::s4u::ElasticTaskManager>("compose_post_service",v_serv_compose_post_service);
    serv_compose_post_service->setBootDuration(0);
    serv_compose_post_service->setParallelTasksPerInst(1);
    serv_compose_post_service->setOutputFunction(return_compose_post_service);
    serv_compose_post_service->setExecAmountFunc(pr_compose_post_service);
    serv_compose_post_service->setReqNames(reqTypeToStr);
        if (m.find("compose_post_service") != m.end()) {
        posS = m.find("compose_post_service")->second;
    }else{
        posS = m.find("default")->second;
    }
    XBT_INFO("compose_post_service on %s", posS.c_str());
    serv_compose_post_service->addHost(simgrid::s4u::Host::by_name(posS));
    simgrid::s4u::Actor::create("etm_compose_post_service", simgrid::s4u::Host::by_name(posS), [serv_compose_post_service] { serv_compose_post_service->run(); });


    // create ETM for service unique_id_service
    std::vector<std::string> v_serv_unique_id_service = std::vector<std::string>();
    v_serv_unique_id_service.push_back("unique_id_service");
    std::shared_ptr<simgrid::s4u::ElasticTaskManager> serv_unique_id_service = std::make_shared<simgrid::s4u::ElasticTaskManager>("unique_id_service",v_serv_unique_id_service);
    serv_unique_id_service->setBootDuration(0);
    serv_unique_id_service->setParallelTasksPerInst(1);
    serv_unique_id_service->setOutputFunction(return_unique_id_service);
    serv_unique_id_service->setExecAmountFunc(pr_unique_id_service);
    if (m.find("unique_id_service") != m.end()) {
        posS = m.find("unique_id_service")->second;
    }else{
        posS = m.find("default")->second;
    }
    XBT_INFO("unique_id_service on %s", posS.c_str());
    serv_unique_id_service->addHost(simgrid::s4u::Host::by_name(posS));
    simgrid::s4u::Actor::create("etm_unique_id_service", simgrid::s4u::Host::by_name(posS), [serv_unique_id_service] { serv_unique_id_service->run(); });


    // create ETM for service media_service
    std::vector<std::string> v_serv_media_service = std::vector<std::string>();
    v_serv_media_service.push_back("media_service");
    std::shared_ptr<simgrid::s4u::ElasticTaskManager> serv_media_service = std::make_shared<simgrid::s4u::ElasticTaskManager>("media_service",v_serv_media_service);
    serv_media_service->setBootDuration(0);
    serv_media_service->setParallelTasksPerInst(1);
    serv_media_service->setOutputFunction(return_media_service);
    serv_media_service->setExecAmountFunc(pr_media_service);
    serv_media_service->setReqNames(reqTypeToStr);
    if (m.find("media_service") != m.end()) {
        posS = m.find("media_service")->second;
    }else{
        posS = m.find("default")->second;
    }
    XBT_INFO("media_service on %s", posS.c_str());
    serv_media_service->addHost(simgrid::s4u::Host::by_name(posS));
    simgrid::s4u::Actor::create("etm_media_service", simgrid::s4u::Host::by_name(posS), [serv_media_service] { serv_media_service->run(); });


    // create ETM for service user_service
    std::vector<std::string> v_serv_user_service = std::vector<std::string>();
    v_serv_user_service.push_back("user_service");
    std::shared_ptr<simgrid::s4u::ElasticTaskManager> serv_user_service = std::make_shared<simgrid::s4u::ElasticTaskManager>("user_service",v_serv_user_service);
    serv_user_service->setBootDuration(0);
    serv_user_service->setParallelTasksPerInst(1);
    serv_user_service->setOutputFunction(return_user_service);
    serv_user_service->setExecAmountFunc(pr_user_service);
    serv_user_service->setReqNames(reqTypeToStr);
    if (m.find("user_service") != m.end()) {
        posS = m.find("user_service")->second;
    }else{
        posS = m.find("default")->second;
    }
    XBT_INFO("user_service on %s", posS.c_str());
    serv_user_service->addHost(simgrid::s4u::Host::by_name(posS));
    simgrid::s4u::Actor::create("etm_user_service", simgrid::s4u::Host::by_name(posS), [serv_user_service] { serv_user_service->run(); });


    // create ETM for service text_service
    std::vector<std::string> v_serv_text_service = std::vector<std::string>();
    v_serv_text_service.push_back("text_service");
    std::shared_ptr<simgrid::s4u::ElasticTaskManager> serv_text_service = std::make_shared<simgrid::s4u::ElasticTaskManager>("text_service",v_serv_text_service);
    serv_text_service->setBootDuration(0);
    serv_text_service->setParallelTasksPerInst(1);
    serv_text_service->setOutputFunction(return_text_service);
    serv_text_service->setExecAmountFunc(pr_text_service);
    serv_text_service->setReqNames(reqTypeToStr);
    if (m.find("text_service") != m.end()) {
        posS = m.find("text_service")->second;
    }else{
        posS = m.find("default")->second;
    }
    XBT_INFO("text_service on %s", posS.c_str());
    serv_text_service->addHost(simgrid::s4u::Host::by_name(posS));
    simgrid::s4u::Actor::create("etm_text_service", simgrid::s4u::Host::by_name(posS), [serv_text_service] { serv_text_service->run(); });


    // create ETM for service user_mention_service
    std::vector<std::string> v_serv_user_mention_service = std::vector<std::string>();
    v_serv_user_mention_service.push_back("user_mention_service");
    std::shared_ptr<simgrid::s4u::ElasticTaskManager> serv_user_mention_service = std::make_shared<simgrid::s4u::ElasticTaskManager>("user_mention_service",v_serv_user_mention_service);
    serv_user_mention_service->setBootDuration(0);
    serv_user_mention_service->setParallelTasksPerInst(1);
    serv_user_mention_service->setOutputFunction(return_user_mention_service);
    serv_user_mention_service->setExecAmountFunc(pr_user_mention_service);
    serv_user_mention_service->setReqNames(reqTypeToStr);
    if (m.find("user_mention_service") != m.end()) {
        posS = m.find("user_mention_service")->second;
    }else{
        posS = m.find("default")->second;
    }
    XBT_INFO("user_mention_service on %s", posS.c_str());
    serv_user_mention_service->addHost(simgrid::s4u::Host::by_name(posS));
    simgrid::s4u::Actor::create("etm_user_mention_service", simgrid::s4u::Host::by_name(posS), [serv_user_mention_service] { serv_user_mention_service->run(); });


    // create ETM for service home_timeline_service
    std::vector<std::string> v_serv_home_timeline_service = std::vector<std::string>();
    v_serv_home_timeline_service.push_back("home_timeline_service");
    std::shared_ptr<simgrid::s4u::ElasticTaskManager> serv_home_timeline_service = std::make_shared<simgrid::s4u::ElasticTaskManager>("home_timeline_service",v_serv_home_timeline_service);
    serv_home_timeline_service->setBootDuration(0);
    serv_home_timeline_service->setParallelTasksPerInst(1);
    serv_home_timeline_service->setOutputFunction(return_home_timeline_service);
    serv_home_timeline_service->setExecAmountFunc(pr_home_timeline_service);
    serv_home_timeline_service->setReqNames(reqTypeToStr);
    if (m.find("home_timeline_service") != m.end()) {
        posS = m.find("home_timeline_service")->second;
    }else{
        posS = m.find("default")->second;
    }
    XBT_INFO("home_timeline_service on %s", posS.c_str());
    serv_home_timeline_service->addHost(simgrid::s4u::Host::by_name(posS));
    simgrid::s4u::Actor::create("etm_home_timeline_service", simgrid::s4u::Host::by_name(posS), [serv_home_timeline_service] { serv_home_timeline_service->run(); });


    // create ETM for service social_graph_service
    std::vector<std::string> v_serv_social_graph_service = std::vector<std::string>();
    v_serv_social_graph_service.push_back("social_graph_service");
    std::shared_ptr<simgrid::s4u::ElasticTaskManager> serv_social_graph_service = std::make_shared<simgrid::s4u::ElasticTaskManager>("social_graph_service",v_serv_social_graph_service);
    serv_social_graph_service->setBootDuration(0);
    serv_social_graph_service->setParallelTasksPerInst(1);
    serv_social_graph_service->setOutputFunction(return_social_graph_service);
    serv_social_graph_service->setExecAmountFunc(pr_social_graph_service);
    serv_social_graph_service->setReqNames(reqTypeToStr);
    if (m.find("social_graph_service") != m.end()) {
        posS = m.find("social_graph_service")->second;
    }else{
        posS = m.find("default")->second;
    }
    XBT_INFO("social_graph_service on %s", posS.c_str());
    serv_social_graph_service->addHost(simgrid::s4u::Host::by_name(posS));
    simgrid::s4u::Actor::create("etm_social_graph_service", simgrid::s4u::Host::by_name(posS), [serv_social_graph_service] { serv_social_graph_service->run(); });


    // create ETM for service user_timeline_service
    std::vector<std::string> v_serv_user_timeline_service = std::vector<std::string>();
    v_serv_user_timeline_service.push_back("user_timeline_service");
    std::shared_ptr<simgrid::s4u::ElasticTaskManager> serv_user_timeline_service = std::make_shared<simgrid::s4u::ElasticTaskManager>("user_timeline_service",v_serv_user_timeline_service);
    serv_user_timeline_service->setBootDuration(0);
    serv_user_timeline_service->setParallelTasksPerInst(1);
    serv_user_timeline_service->setOutputFunction(return_user_timeline_service);
    serv_user_timeline_service->setExecAmountFunc(pr_user_timeline_service);
    serv_user_timeline_service->setReqNames(reqTypeToStr);
    if (m.find("user_timeline_service") != m.end()) {
        posS = m.find("user_timeline_service")->second;
    }else{
        posS = m.find("default")->second;
    }
    XBT_INFO("user_timeline_service on %s", posS.c_str());
    serv_user_timeline_service->addHost(simgrid::s4u::Host::by_name(posS));
    simgrid::s4u::Actor::create("etm_user_timeline_service", simgrid::s4u::Host::by_name(posS), [serv_user_timeline_service] { serv_user_timeline_service->run(); });


    // create ETM for service post_storage_service
    std::vector<std::string> v_serv_post_storage_service = std::vector<std::string>();
    v_serv_post_storage_service.push_back("post_storage_service");
    std::shared_ptr<simgrid::s4u::ElasticTaskManager> serv_post_storage_service = std::make_shared<simgrid::s4u::ElasticTaskManager>("post_storage_service",v_serv_post_storage_service);
    serv_post_storage_service->setBootDuration(0);
    serv_post_storage_service->setParallelTasksPerInst(1);
    serv_post_storage_service->setOutputFunction(return_post_storage_service);
    serv_post_storage_service->setExecAmountFunc(pr_post_storage_service);
    serv_post_storage_service->setReqNames(reqTypeToStr);
    if (m.find("post_storage_service") != m.end()) {
        posS = m.find("post_storage_service")->second;
    }else{
        posS = m.find("default")->second;
    }
    XBT_INFO("post_storage_service on %s", posS.c_str());
    serv_post_storage_service->addHost(simgrid::s4u::Host::by_name(posS));
    simgrid::s4u::Actor::create("etm_post_storage_service", simgrid::s4u::Host::by_name(posS), [serv_post_storage_service] { serv_post_storage_service->run(); });


    // create ETM for service url_shorten_service
    std::vector<std::string> v_serv_url_shorten_service = std::vector<std::string>();
    v_serv_url_shorten_service.push_back("url_shorten_service");
    std::shared_ptr<simgrid::s4u::ElasticTaskManager> serv_url_shorten_service = std::make_shared<simgrid::s4u::ElasticTaskManager>("url_shorten_service",v_serv_url_shorten_service);
    serv_url_shorten_service->setBootDuration(0);
    serv_url_shorten_service->setParallelTasksPerInst(1);
    serv_url_shorten_service->setOutputFunction(return_url_shorten_service);
    serv_url_shorten_service->setExecAmountFunc(pr_url_shorten_service);
    serv_url_shorten_service->setReqNames(reqTypeToStr);
        if (m.find("url_shorten_service") != m.end()) {
        posS = m.find("url_shorten_service")->second;
    }else{
        posS = m.find("default")->second;
    }
    XBT_INFO("url_shorten_service on %s", posS.c_str());
    serv_url_shorten_service->addHost(simgrid::s4u::Host::by_name(posS));
    simgrid::s4u::Actor::create("etm_url_shorten_service", simgrid::s4u::Host::by_name(posS), [serv_url_shorten_service] { serv_url_shorten_service->run(); });

  /* ADD DATASOURCES MANUALLY HERE, SET THE END TIMER AS YOU WISH, AND LAUNCH YOUR SIMULATOR*/
  DataSourceFixedInterval* dsf = new DataSourceFixedInterval("nginx_web_server",RequestType::COMPOSE, 1/freq,100);
  simgrid::s4u::ActorPtr dataS = simgrid::s4u::Actor::create("snd", simgrid::s4u::Host::by_name("clemth.irisa.fr"), [&]{dsf->run();});

  // kill policies and ETMs
  simgrid::s4u::this_actor::sleep_for(30); /*set it according to your needs*/
  XBT_INFO("Done. Killing policies and etms");
      dataS->kill();
    serv_nginx_web_server->kill();
    serv_compose_post_service->kill();
    serv_unique_id_service->kill();
    serv_media_service->kill();
    serv_user_service->kill();
    serv_text_service->kill();
    serv_user_mention_service->kill();
    serv_home_timeline_service->kill();
    serv_social_graph_service->kill();
    serv_user_timeline_service->kill();
    serv_post_storage_service->kill();
    serv_url_shorten_service->kill();
}
int main(int argc, char* argv[]) {
	simgrid::s4u::Engine* e = new simgrid::s4u::Engine(&argc, argv);
      // create a map from the file
      std::string fname = argv[2];
      std::ifstream input(fname);
      std::map<std::string, std::string> pos;
      for (std::string token1, token2; input >> token1 >> token2;) {
            pos[token1] = token2;
      }
	e->load_platform(argv[1]);
	simgrid::s4u::Actor::create("main", simgrid::s4u::Host::by_name("clemth.irisa.fr"), [&]{run(pos, std::stod(argv[3]));});
	e->run();
	return 0;
}
