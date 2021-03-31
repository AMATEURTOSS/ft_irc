#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <vector>
#include "Member.hpp"

// channel의 이름은 최대 50자이며 대소문자를 구분하지 않고 &, #, +, !로 시작함
// 공백, ctrl+G, 쉼표를 포함하지 않아야 함
// 콜론은 채널 마스크의 구분 기호로 사용
// key	=  1*23( %x01-05 / %x07-08 / %x0C / %x0E-1F / %x21-7F )
//			any 7-bit US_ASCII character,
//			except NUL, CR, LF, FF, h/v TABs, and " "
// 자세한 사항은 IRC 서버 프로토콜 참조
// 접두어 &가 있는 채널은 생성된 서버에 로컬
// 접두어 +는 채널 모드를 지원하지 않음(t채널 플래그를 제외하고 모든 모드가 설정되지 않음)
// 채널 마스크에 따라 하나 이상의 서버에 알려짐. 없는 경우는 모든 서버에 알려져 있음
// 채널 마스크가 있는 경우 채널은 채널에 로컬 사용자가 있는 서버, 마스크가 로컬 및 인접 서버 이름과 모두 일치하는 경우 인접 서버에 알려야 함
// 채널 마스크는 서버 호스트 마스킹과 함께 사용하는것이 가장 좋음(irc 서버 프로토콜 참조)

// 채널 운영자/생성자는 INVITE, KICK, MODE, PRIVMSG, TOPIC

/*
| O | "채널 생성자" 상태 제공| - 권한 없음
| o | 채널 운영자 권한 부여/받기 |
| v | 음성 권한 부여/받기 |
| a | 익명 채널 플래그를 토글 | - 권한 낮음
| i | 초대 전용 채널 플래그를 전환 | - invite를 가진 유저만 채널에 접속 가능, invite는 일회성
| m | moderated 채널을 전환 | - creator, operator, voice 권한이 있는 유저만 privmsg를 보낼 수 있음
| n | 외부의 클라이언트에서 채널로 메시지 없음을 전환 | - 해당 채널 멤버만 메시지 전송 가능
| q | 조용한 채널 플래그를 토글 | - 권한 낮음(채널 생성자 상태에서 테스트함)
| p | private 채널 플래그를 토글 | - 동작x
| s | 비밀 채널 플래그를 토글 | - 해당 채널에 접속한 유저만 채널이 표시 됨(whois, list 등등)
| r | 서버 reop 채널 플래그를 토글 | - 안전 채널에서만 사용 가능
| t | 채널 운영자 전용 플래그로 설정 가능한 주제를 토글 | - 말 그대로 운영자만 토픽 설정 가능함
| k | 채널 키(비밀번호)를 설정/제거 | - 
| l(L) | 채널에 대한 사용자 제한을 설정/제거 | - k <key> or -k
| b | 사용자를 차단하기 위해 차단 마스크를 설정/제거 | - b <mask> or -b
| e | 차단 마스크를 무시하기 위해 예외 마스크를 설정/제거 | - 해당 마스크에 일치하는 유저는 ban mask에 있어도 접속 가능
| I(i) | 초대 전용 플래그를 자동으로 무시하도록 초대 마스크를 설정/제거 | - 해당 마스크에 일치하는 유저는 invite가 없어도 접속 가능
*/

// 채널 생성한 멤버에 관리자/생성자 권한 부여


class Channel
{
private:
	const std::string				_name;
	std::string						_key;
	std::string						_topic;
	std::vector<Member *>			_member;
	std::vector<std::string>		_ban_list;
	std::vector<Member *>			_operators;
	int								_mode;
	int								_limit;
	//								_properties.op_members;
	//								_properties.create_member;
	//								...
	//								_properties.mode;
	
public:
	Channel(const std::string channel_name, const std::string key, Member *first_member);
	Channel(const std::string channel_name, Member *first_member);
	Channel(const Channel & other);
	Channel & operator = (const Channel & other);
	~Channel();
public:
	void					add_member(Member *member);
	int						delete_member(Member *member);
	std::vector<Member *>	get_members();
	const std::string 		&get_name();
	bool					find_member(Member * member);
	bool					set_topic(std::string const &topic);
	std::string				get_topic();
	bool					check_mode(char mode, bool is_set);
	int						get_mode();
	void					set_mode(int mode);
	int						get_limit();
	void					set_limit(int limit);
};

#endif