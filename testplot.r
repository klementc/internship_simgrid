library(ggplot2)
library(dplyr)
library(tidyr)
library(tikzDevice)
library(gridExtra)

getwd()
setwd("/home/clem/Code/github.com/klementc/internship_simgrid")
data <- read.csv("./analysis/exposimple/datainter3.csv")

gi <- ggplot(data, aes(x=timestamp, y=nInstances))+
  geom_point(size=1)+
  geom_line(color="blue")
plot(gi)

gl <- ggplot(data, aes(x=timestamp, y=load))+
  geom_point(size=1)+
  geom_line(color="red")
plot(gl)

grid.arrange(gi,gl)

##########################

data <- read.csv("./test.csv")
data2 <- read.csv("./default1ArrivalRates.csv")
data$totalReqInSys <- data$waitingReq + data$executingReq

gh <-ggplot(data2, aes(x=time, y=rate))+
  geom_line(color="orange",size=2)
plot(gh)

gi <- ggplot(data, aes(x=timestamp, y=nInstances))+
  geom_point(size=1)+
  geom_line(color="blue")+
  facet_wrap("policy", scales="free")
plot(gi)

gl <- ggplot(data, aes(x=timestamp, y=load))+
  geom_point(size=.5)+
  geom_line(color="red", size=.5)+
  facet_wrap("policy", scales="fixed")
plot(gl)

gm <- ggplot(data, aes(x=timestamp, y=(waitingReq)))+
  geom_point(size=.5)+
  geom_line(color="green", size=.5)+
  facet_wrap("policy", scales="free", )
plot(gm)


# gn <- ggplot(data, aes(x=timestamp, y=(executingReq)))+
#   geom_point(size=.5)+
#   geom_line(color="green", size=.5)+
#   facet_wrap("policy", scales="free", )
# plot(gm)

#data$cap <- (data$nInstances*8)*10
#data$totr <- data$executingReq+data$execInSlot
gn <- ggplot(data, aes(x=timestamp))+
  geom_line(aes(y=execInSlot), color = "red") +
  geom_line(aes(y=parPerInst), color = "blue") +geom_histogram(data=data2,binwidth=10)
plot(gn)

grid.arrange(gh,gi,gl,gm,gn)


########## FIFA


data2 <- read.csv("./analysis/FIFAtrace/t.csv")

gh <- ggplot(data2, aes(x=timestamps))+geom_histogram(binwidth=1)
plot(gh)


d <- read.csv("./test.csv")

gi <- ggplot(d, aes(x=timestamp, y=nInstances))+
  geom_point(size=1)+
  geom_line(color="blue")+
  facet_wrap("policy", scales="free")+
  expand_limits(y = 0)
plot(gi)

gl <- ggplot(d, aes(x=timestamp, y=load))+
  geom_point(size=.5)+
  geom_line(color="red", size=.5)+
  facet_wrap("policy", scales="fixed")+
  expand_limits(y = 0)
plot(gl)

gm <- ggplot(d, aes(x=timestamp, y=(waitingReq)))+
  geom_point(size=.5)+
  geom_line(color="green", size=.5)+
  facet_wrap("policy", scales="free", )+
  expand_limits(y = 0)
plot(gm)


# gn <- ggplot(data, aes(x=timestamp, y=(executingReq)))+
#   geom_point(size=.5)+
#   geom_line(color="green", size=.5)+
#   facet_wrap("policy", scales="free", )
# plot(gm)

d$ta <- (d$execInSlot)/60
d$tb <- d$parPerInst/60
gn <- ggplot(NULL, aes())+
  geom_line(data=d, aes(y=ta,x=timestamp), color = "red",size=2,alpha=.5) +
  geom_line(data=d, aes(y=tb,x=timestamp), color = "blue") +
  geom_histogram(data=data2,aes(x=timestamps),binwidth=1, alpha=.5)
plot(gn)

grid.arrange(gh,gi,gl,gm,gn)
