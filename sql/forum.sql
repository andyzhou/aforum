#创建db
CREATE DATABASE af DEFAULT CHARACTER SET utf8;


#语言表
create table af_lan (
id mediumint(8) unsigned not null auto_increment,
appid smallint(5) unsigned not null default 0,#应用id(必须) 例如:系统、论坛....
ltype smallint(3) unsigned not null default 0,#语言类型 0:中文
lid smallint(5) unsigned not null default 0,#语言标识id
ldesc varchar(255) binary not null default '',#语言内容
index alid(appid, ltype, lid),
primary key(id)
)TYPE=MyISAM PACK_KEYS=1 DEFAULT CHARACTER SET utf8;



#专题分类表
create table af_doc_cat (
id smallint(5) unsigned not null auto_increment,
title varchar(100) binary not null default '',
intro text,
docs mediumint(8) unsigned not null default 0,
primary key(id)
)TYPE=MyISAM PACK_KEYS=1 DEFAULT CHARACTER SET utf8;


#专题(文章)表
create table af_docs (
id mediumint(8) unsigned not null auto_increment,
cid smallint(5) unsigned not null default 0,
title varchar(128) binary not null default '',
author varchar(64) binary not null default '',
logo varchar(64) binary not null default '',#logo图
link varchar(200) binary not null default '',#外部链接
intro text,
detail text,
uptime date,
subs smallint(3) unsigned not null default 0,#子分页数
issub tinyint(2) unsigned not null default 0,#是否为子分页
luptime mediumint(8) unsigned not null default 0,#YYMMDD
index csid(cid, issub),
index luptime(luptime),
primary key(id)
)TYPE=MyISAM PACK_KEYS=1 DEFAULT CHARACTER SET utf8;



#版主|权限表 2007-10-29
create table af_moderator (
bname char(32) binary not null default '',#板块标识
uid mediumint(8) unsigned not null default 0,#用户id
name char(32) binary not null default '',#用户昵称
mtype tinyint(2) unsigned not null default 0,#版主类型 0:主 1:副 2:分舵
mboard tinyint(2) unsigned not null default 0,#修改版块信息 0:无权 1:有权
dtopic tinyint(2) unsigned not null default 1,#删除主题 0:无 1:有
ttopic tinyint(2) unsigned not null default 1,#置顶主题 0:无 1:有
htopic tinyint(2) unsigned not null default 1,#精华主题 0:无 1:有
ltopic tinyint(2) unsigned not null default 1,#锁定主题 0:无 1:有
unique key(bname, uid)
)TYPE=MyISAM PACK_KEYS=1 DEFAULT CHARACTER SET utf8;


#类别&版块
create table af_boards (
id mediumint(8) unsigned not null auto_increment,
title varchar(128) binary not null default '',#版块标题
bname varchar(32) binary not null default '',#板块标识，英文(＋数字)
pid smallint(5) unsigned not null default 0,#根版块id
topics mediumint(8) unsigned not null default 0,#主题数
posts mediumint(8) unsigned not null default 0,#帖子数
intro text,#版块介绍
isOpen tinyint(1) unsigned not null default 0,#是否开启
browLevel tinyint(1) unsigned not null default 0,#默认所有人 1:会员 2:版主 3:管理员
postLevel tinyint(1) unsigned not null default 1,#默认注册会员 1:会员 2:版主 3:管理员
passwd char(10) binary not null default '',#访问密码
viewType tinyint(1) unsigned not null default 1,#浏览方式1 板式 2 树状
sortNum smallint(5) unsigned not null default 1,#板块排序
fcat mediumint(8) unsigned not null default 0,#所属板块id
uploadType varchar(64) binary not null default '',#上载文件类型
moderator varchar(255) binary not null default '',#版主列表
lastPost varchar(128) binary not null default '',#最后更新
lastUpdate mediumint(8) unsigned not null default 0,#最后更新时间 YYMMDD

index ppsort(isOpen, fcat, sortNum),#该索引效果更佳，支持where、order by
index bname(bname),
primary key(id)
)TYPE=MyISAM PACK_KEYS=1 DEFAULT CHARACTER SET utf8;





#用于板状(树状)显示主题，单个板块一个表
create table af_topic_$boardId (
id mediumint(8) unsigned not null auto_increment,#主题ID
title char(128) binary default null default '',#标题
express tinyint(2) unsigned not null default 0,#表情
hit mediumint(8) unsigned not null default 0,#点击数
reply smallint(5) unsigned not null default 0,#回复数
hasAtt tinyint(2) unsigned not null default 0,#是否有附件 0:没有 1:有
attType tinyint(2) unsigned not null default 0,#附件 0:无 1:图片 2:文本 3:媒体 5:压缩 6:其他
len mediumint(8) unsigned not null default 0,#内容长度

hot tinyint(2) unsigned not null default 0,#精华
locked tinyint(2) unsigned not null default 0,#锁定
pid mediumint(8) unsigned not null default 0,#父贴id
rid mediumint(8) unsigned not null default 0,#根贴id
lay tinyint(3) unsigned not null default 0,#帖子层次
ord smallint(5) unsigned not null default 0,#帖子排序值

poster char(32) binary not null default '',#发贴人
posterId mediumint(8) unsigned not null default 0,#发贴人ID

lastPoster char(32) binary not null default '',#最后发贴人
lastPosterId mediumint(8) unsigned not null default 0,#最后发贴人ID

ft tinyint(2) unsigned not null default 0,#是否为板状主题 0:不是 1:是
tt tinyint(2) unsigned not null default 0,#是否为树状主题 :不是 1:是 当为新根贴时，ft、tt均为1

postTime char(20) binary not null default '',#主题发表时间,时间戳。
lastPost int(10) unsigned not null default 0,#最后回复时间,时间戳。

index tsort(tt, rid, lastPost, ord),#树状列表索引(效果更好)
index fsort(ft, lastPost),#板状列表索引
index fhsort(ft, hit),#状列按点击数排序索引
index frsort(ft, reply),#状列按回复数排序索引
index tlast(lastPost),
primary key(id)
) TYPE=MyISAM PACK_KEYS=1 DEFAULT CHARACTER SET utf8;


#用于主题阅读，单个板块一个表
create table af_content_$boardId (
id mediumint(8) unsigned not null default 0,#帖子ID, 与主题中id一致
title varchar(128) binary default null default '',#标题
express tinyint(2) unsigned not null default 0,#表情
rid mediumint(8) unsigned not null default 0,#根主题ID
hasAtt tinyint(2) unsigned not null default 0,#是否有附件 0:没有 > 0:附件数
attType tinyint(2) unsigned not null default 0,#附件 0:无 1:图片 2:文本 3:媒体 5:压缩 6:其他
attList varchar(255) binary not null default '',#附件列表
hot tinyint(2) unsigned not null default 0,#精华
locked tinyint(2) unsigned not null default 0,#锁定
content text,#帖子内容
posterId mediumint(8) unsigned not null default 0,#发贴人ID
poster varchar(32) binary not null default '',#发帖人
postTime varchar(20) binary not null default '',#主题发表时间

index rid(rid),
index posterId(posterId),
index title(title),
primary key(id)
)TYPE=MyISAM PACK_KEYS=1 DEFAULT CHARACTER SET utf8;

##全文搜索SQL

SELECT 表字段 FROM 表名 WHERE MATCH (全文搜索表字段) AGAINST ('搜索字符串');



#精华贴
create table af_hot (
id mediumint(8) unsigned not null auto_increment,
boardId mediumint(8) unsigned not null default '0',#板块id
title char(128) binary not null default '',#精华标题
topicId mediumint(8) unsigned not null default 0,#精华相关主题id
poster char(32) binary not null default '',#发贴人
posterId mediumint(8) unsigned not null default 0,#发贴人ID
postTime int(10) unsigned not null default 0,#发布时间(时间戳)

index bsid(boardId, postTime),
primary key(id)
)TYPE=MyISAM PACK_KEYS=1 DEFAULT CHARACTER SET utf8;



#用户关注相关数据, 提高用户凝聚力
create table af_atten (
id mediumint(8) unsigned not null auto_increment,
attenType tinyint(2) unsigned not null default 0,#关注数据类型 0:主题 1:用户 2:板块
uid mediumint(8) unsigned not null default 0,#用户id
bid mediumint(8) unsigned not null default 0,#关注的主题所在板块id
aoid mediumint(8) unsigned not null default 0,#关注对象id(主题id、用户id、板块id)
title varchar(128) binary not null default '',#关注的标题，用户名等信息
attenTime int(10) unsigned not null default 0,#关注时间(时间戳)

index tsid(attenType, uid, attenTime),
primary key(id)
)TYPE=MyISAM PACK_KEYS=1;


#会员表
create table af_member (
id mediumint(8) unsigned not null auto_increment,
uname char(32) binary not null default '',#用户名
passwd char(32) binary not null default '',#密码
name char(32) binary not null default '',#用户昵称
email char(40) binary not null default '',#个人邮件

myPhoto char(32) binary not null default '',#个人照片
sex tinyint(2) unsigned not null default 0,#性别 0:保密 1:男 2:女
location char(64) binary not null default '',#所在地区
signInfo char(128) binary not null default '',#个人签名

posts mediumint(8) unsigned not null default 0,#发表主题数
pms mediumint(8) unsigned not null default 0,#短消息数
score mediumint(8) unsigned not null default 0,#积分
experience mediumint(8) unsigned not null default 0,#经验值
userType tinyint(2) unsigned not null default 1,#1.会员 2.贵宾 3.版主 4.管理员

regDate date,#注册时间
lastLogin datetime,#最后登录时间
ban tinyint(2) unsigned not null default 0,#是否禁止发言
banBeginTime mediumint(8) unsigned not null default 0,#禁止发言开始时间
banEndTime mediumint(8) unsigned not null default 0,#禁止发言结束时间

index uname(uname),
primary key(id)
)TYPE=MyISAM PACK_KEYS=1 DEFAULT CHARACTER SET utf8;




#站内消息表
create table af_pm (
id int(10) unsigned not null auto_increment,
title varchar(128) binary not null default '',#标题
content varchar(255) binary not null default '',#详细内容
sender varchar(32) binary not null default '',#发送人
sendId mediumint(8) unsigned not null default 0,#发送人ID
revId mediumint(8) unsigned not null default 0,#接收人ID
isNew tinyint(2) unsigned not null default 0,#是否为新留言 0:否 1:是
sendTime int(10) unsigned not null default 0,#发送时间(时间戳)

index rsid(revId, isNew, sendTime),
primary key(id)
)TYPE=MyISAM PACK_KEYS=1;



#系统公告|置顶贴(系统、各个板块)
create table af_announce (
id mediumint(8) unsigned not null auto_increment,
annType int(3) unsigned not null default '0',#公告类型, 0:系统 1:版块
catId mediumint(8) unsigned not null default '1',#类别id
boardId mediumint(8) unsigned not null default '0',#板块id
title char(128) binary not null default '',#公告标题
topicId mediumint(8) unsigned not null default 0,#公告相关主题id
postTime int(10) unsigned not null default 0,#发布时间(时间戳)

index tbsid(annType, catId, boardId),
primary key(id)
)TYPE=MyISAM PACK_KEYS=1;



#投票表
create table af_poll (
id mediumint(8) unsigned not null auto_increment,
boardId mediumint(8) unsigned not null default 0,#投票所在板块id
topicId mediumint(8) unsigned not null default 0,#投票所属主题id
ptype tinyint(2) unsigned not null default 0,#投票类型 0:单选 1:多选
options char(255) binary not null default '',#投票选择项列表 a|b|c
result char(128) binary not null default '',#投票统计结果值 1|2|3
beginTime mediumint(8) unsigned not null default 0,#开始日期 YMMDD
endTime mediumint(8) unsigned not null default 0,#停止日期 YMMDD

index bsid(boardId, topicId),
primary key(id)
)TYPE=MyISAM PACK_KEYS=1;



#投票参与记录
create table af_poll_log (
id int(10) unsigned not null auto_increment,
pid mediumint(8) unsigned not null default 0,#投票记录id
uid mediumint(8) unsigned not null default 0,#用户id
choose varchar(32) binary not null default '',#选择值
index pusid(pid, uid),
primary key(id)
)TYPE=MyISAM PACK_KEYS=1;

