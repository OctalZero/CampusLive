/*****************************************************************************
  *Date:  2022.06.13
  *Description:  定义的标识类，里面包含定义功能的枚举类型（用于Json的信息传输）
*****************************************************************************/
#pragma once
#include <QString>

//----------------------------------------
// 定义 Client 的身份标识
namespace  ClientIdentification {
	typedef enum {
		Teacher,  // 教师
		Student,  // 学生
	};
}
//----------------------------------------


//----------------------------------------
// 定义消息转发给哪个控制器对象
namespace MessageForwordControl {
	typedef enum {
		GetInfo,  // 获取信息控制器
		SetInfo,  // 设置信息控制器
		Login,  // 登录界面控制器
		Register,  // 注册界面控制器
		Homepage,  // 首页控制器
		Classroom,  // 课堂界面控制器
	};
}
// 定义每个控制器类型处理的消息类型
namespace Login {
	typedef enum {
		// Client 类型:
		Login,  // 进行登录

		// Server 类型:
		LoginResult,  // 返回登录结果
	};
}
namespace Register {
	typedef enum {
		// Client 类型:
		Register,  // 进行注册

		// Server 类型：
		RegisterResult,  // 返回注册结果
	};
}
namespace Homepage {
	typedef enum {
		// Client 类型：
		InitHomepage,  // 初始化首页
		AddCourse,  // 学生加入课堂
		CreateCourse, // 教师创建课程
		DeselectionCourse,// 学生退选课程
		DeleteCourse, // 教师删除课程
		QueryCourseDetails,  // 查询课程详情
		RfreshCourseStatus,  // 刷新课程状态

// Server 类型:
InitHomepageResult,  // 返回初始化首页结果
AddCourseResult,  // 添加课程返回结果
CreateCourseResult,  // 教师创建课程返回结果
DeselectionCourseResult, // 学生退选课程返回结果
DeleteCourseResult, // 教师删除课程返回结果
QueryCourseDetailsResult,  // 查询课程详情返回结果
RfreshCourseStatusResult,  // 刷新课程状态返回结果
	};
}
namespace Classroom {
	typedef enum {
		// Client 类型：
		InitClassroom,  // 初始化首页
		SendChatMessage, // 发送聊天消息
		ExitClassroom,  // 学生退出课堂
		CloseClassroom, // 教师关闭课堂

		// Server 类型:
		InitClassroomResult,  // 返回初始化教室结果
		UpdateChatMessage, // 更新聊天消息
		InitClassMembers,  // 初始化课堂成员信息
		UpdateClassMembers, // 更新进入课堂的成员信息
		CloseClassroomResult, // 教师关闭课堂结果
	};
}
namespace GetInfo {
	typedef enum {
		// Client 类型:
		GetCoursesAbstract,  // 获取课程概要信息
		GetClassInfo, // 获取课堂信息
	};
}
namespace SetInfo {
	typedef enum {
		// Client 类型:
		SetStudentInfo,  // 设置学生信息
		AddClassInfo,  // 添加课堂信息

		// Server 类型:
		UpdateCourse,  // 更新课程
		AddCourse,  // 添加课程
		DeleteCourse,  // 删除课程
		UpdateClassMemberAbstract,  // 更新课堂成员概要
	};
}
//----------------------------------------

//-----------------------------------------
// 定义各种状态
// 客户端状态（Server 使用）
namespace  ClientStatus {
	typedef enum {
		OffLine,  // 用户不在线
		OnLine,  //用户在线
	};
}
// 课程状态
namespace  CourseStatus {
	typedef enum {
		NotInClass, // 未上课
		InClass,  // 正在上课
	};
}
//-----------------------------------------

//-----------------------------------------
// 定义各种信息的数据结构
namespace InfoStruct {
	// 课堂成员概要信息
	struct ClassMemberAbstract
	{
		QString id;  // 成员账号
		QString identification;  // 成员身份标识
		QString name;  // 成员姓名
	};
	// 学生课程概要信息
	struct CourseAbstract
	{
		QString id;  // 课程号
		QString name;  // 课程名
		quint16 status;  // 课程状态
	};
}
//-----------------------------------------