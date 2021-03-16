#ifndef REQUESTTYPE_HPP
#define REQUESTTYPE_HPP

enum class RequestType
{
  DEFAULT,
  COMPOSE,
  REGISTER,
  LOGIN,
  TESTREQ_EXAMPLE,
  /* add whatever you want (not sure how to let the user extend in in a clean way) */
};

#endif /*REQUESTTYPE_HPP*/