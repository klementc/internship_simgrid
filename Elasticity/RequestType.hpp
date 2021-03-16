#ifndef REQUESTTYPE_HPP
#define REQUESTTYPE_HPP

enum class RequestType
{
  DEFAULT,
  COMPOSE,
  REGISTER,
  LOGIN,
  /* add whatever you want (not sure how to let the user extend in in a clean way) */
};

#endif /*REQUESTTYPE_HPP*/