/*
Welcome With Helwan O.S info demo version
*/

/*
 يقوم بتضمين ملف تعريف الإدخال/الإخراج القياسي،
 مما يوفر وظائف مثل printf و scanf لطباعة وقراءة البيانات.
*/
#include <stdio.h> 
/*
 قوم بتضمين ملف تعريف مكتبة القياسي،
 مما يوفر وظائف عامة مختلفة مثل malloc و free لإدارة الذاكرة. 
*/
#include <stdlib.h>
/*
 قوم بتضمين ملف تعريف السلسلة
مما يقدم وظائف لمعالجة السلاسل مثل strcpy و strcmp.
*/
#include <string.h>

#ifdef _WIN32 // this for windows 32 && 64
/*
 مما يوفر الوصول إلى وظائف خاصة بنظام Windows.
*/
#include <windows.h>

#elif __linux__
/*
لاسترداد معلومات النظام مثل اسم المضيف واسم نظام التشغيل 
*/
#include <sys/utsname.h>

/*
وظائف مثل getpid (الحصول على معرف الهوية). 
*/
#include <unistd.h>

#elif __APPLE__
/*
لتكوين النظام والوصول إلى المعلومات 
*/
#include <sys/sysctl.h>
/*
وفر هذه العناوين الوصول إلى وظائف
خاصة بنظام التشغيل macOS للتفاعل مع نواة Mach. 
*/
#include <mach/mach_types.h>
#include <mach/mach_host.h>

/*
للوصول إلى واجهات برمجة التطبيقات القديمة لنظام Macintosh. 
*/
#include <Carbon/Carbon.h>
#endif

int UNLEN = 256;
// تعريف دالة للحصول على معلومات النظام
void get_system_info(char *os_name, char *os_version, char *username) {
 #ifdef _WIN32
 // تعريف هيكل معلومات إصدار نظام التشغيل
 OSVERSIONINFOEX os_info;
 //DWORD size = sizeof(os_info);

 // استدعاء دالة GetVersionEx للحصول على معلومات إصدار نظام التشغيل
 GetVersionEx((OSVERSIONINFO *)&os_info);

 // استخراج اسم نظام التشغيل من هيكل معلومات إصدار نظام التشغيل
 strcpy(os_name, os_info.szCSDVersion);

 // استخراج إصدار نظام التشغيل من هيكل معلومات إصدار نظام التشغيل
 sprintf(os_version, "%ld.%ld.%ld", os_info.dwMajorVersion, os_info.dwMinorVersion, os_info.dwBuildNumber);

 // تعريف متغير لحفظ طول اسم المستخدم
 DWORD username_len = UNLEN + 1;

 // استدعاء دالة GetUserName للحصول على اسم المستخدم
 GetUserName(username, &username_len);
 
 #elif __linux__
 // تعريف هيكل معلومات اسم نظام التشغيل
 struct utsname uname_data;

 // استدعاء دالة uname للحصول على معلومات اسم نظام التشغيل
 uname(&uname_data);

 // استخراج اسم نظام التشغيل من هيكل معلومات اسم نظام التشغيل
 strcpy(os_name, uname_data.sysname);

 // استخراج إصدار نظام التشغيل من هيكل معلومات اسم نظام التشغيل
 strcpy(os_version, uname_data.release);

 // تعريف متغير لحفظ طول اسم المستخدم
 const int UNLEN = 256;

 // استدعاء دالة getlogin_r للحصول على اسم المستخدم
 getlogin_r(username, UNLEN + 1);
 
 #elif __APPLE__
 // تعريف متغيرات لحفظ معلومات النظام
 size_t size;
 char *buffer;

 // استخراج اسم نظام التشغيل
 size = sizeof(os_name);
 sysctlbyname("kern.ostype", os_name, &size, NULL, 0);

 // استخراج إصدار نظام التشغيل
 size = sizeof(os_version);
 sysctlbyname("kern.osrelease", os_version, &size, NULL, 0);

 // استخراج اسم المستخدم
 getlogin_r(username, UNLEN + 1);
 #endif
}


// وظيفة لمعرفة معلومات وحدة المعالجة المركزية

void get_cpu_info(char *cpu_name, int *num_cores) {
 #ifdef _WIN32
 // تعريف هيكل معلومات النظام
 SYSTEM_INFO sys_info;

 // استدعاء دالة GetSystemInfo للحصول على معلومات النظام
 GetSystemInfo(&sys_info);
 // استخراج اسم المعالج من هيكل معلومات النظام
 //strcpy(cpu_name, sys_info.lpProcessorDescription);
 strcpy(cpu_name, sys_info.wProcessorRevision);

 // استخراج عدد النوى من هيكل معلومات النظام
 *num_cores = sys_info.dwNumberOfProcessors;
 

 #elif __linux__
 // فتح ملف /proc/cpuinfo للقراءة
 FILE *cpuinfo = fopen("/proc/cpuinfo", "r");

 // تعريف متغير لحفظ سطر من الملف
 char line[1024];

 // استخراج اسم المعالج من الملف
 while (fgets(line, sizeof(line), cpuinfo)) {
  if (strstr(line, "model name")) {
    sscanf(line, "model name : %[^\n]", cpu_name);
    break;
  }
 }
 // استخراج عدد النوى من الملف
 *num_cores = 0;
 while (fgets(line, sizeof(line), cpuinfo)) {
   if (strstr(line, "cpu cores")) {
     sscanf(line, "cpu cores : %d", num_cores);
     break;
   }
 }

 // إغلاق ملف /proc/cpuinfo
 fclose(cpuinfo);
 
 #elif __APPLE__
 // تعريف متغير لحفظ معلومات المعالج
 ProcessSerialNumber psn;

 // استدعاء دالة Gestalt() للحصول على معلومات المعالج
 Gestalt(gestaltProcInfo, &psn);

 // استخراج اسم المعالج من معلومات النظام
 CFStringRef cpuName = CFStringCreateWithPascalString(kCFAllocatorDefault, &psn.processName, kCFStringEncodingMacRoman);
 CFStringGetCString(cpuName, cpu_name, sizeof(cpu_name), kCFStringEncodingUTF8);
 CFRelease(cpuName);

 // استخراج عدد النوى من معلومات النظام
 int numCores = (int)psn.numberOfProcesses;

 #endif
}

void get_memory_info(int *total_ram) {

 #ifdef _WIN32
 // تعريف هيكل معلومات الذاكرة
 MEMORYSTATUSEX mem_info;

 // تعيين طول هيكل معلومات الذاكرة
 mem_info.dwLength = sizeof(mem_info);

 // استدعاء دالة GlobalMemoryStatusEx للحصول على معلومات الذاكرة
 GlobalMemoryStatusEx(&mem_info);

 // استخراج الذاكرة العشوائية الإجمالية من هيكل معلومات الذاكرة وتحويلها إلى جيجابايت
 //*total_ram = (int)mem_info.ullTotalPhysMem / (1024 * 1024 * 1024);
 *total_ram = (int)mem_info.ullTotalPhys / (1024 * 1024 * 1024);

 #elif __linux__

 // فتح ملف /proc/meminfo للقراءة
 FILE *meminfo = fopen("/proc/meminfo", "r");

 // تعريف متغير لحفظ سطر من الملف
 char line[1024];

 // استخراج الذاكرة العشوائية الإجمالية من الملف
 while (fgets(line, sizeof(line), meminfo)) {
   // البحث عن سطر "MemTotal"
   if (strstr(line, "MemTotal")) {
     // استخراج قيمة الذاكرة العشوائية الإجمالية من السطر
     sscanf(line, "MemTotal: %d kB", total_ram);

     // تحويل قيمة الذاكرة العشوائية الإجمالية من كيلوبايت إلى جيجابايت
     *total_ram /= 1024;

     // إنهاء حلقة while بعد استخراج القيمة المطلوبة
     break;
   }
 }

 // إغلاق ملف /proc/meminfo
 fclose(meminfo);

 #elif __APPLE__
 // تعريف متغيرات لحفظ معلومات الذاكرة
 vm_size_t page_size;
 mach_port_t mach_port;
 vm_statistics_data_t vm_stats;

 // استخراج حجم صفحة الذاكرة
 host_page_size(mach_host_self(), &page_size);

 // استخراج معلومات الذاكرة
 mach_port = mach_host_self();
 if (host_statistics(mach_port, HOST_VM_INFO, (host_info_t)&vm_stats, &info_count) == KERN_SUCCESS) {
   *total_ram = vm_stats.active_memory / page_size;
 }

 #endif
}



int main() {
    char os_name[256], os_version[256], username[256];
    char cpu_name[256];
    int num_cores, total_ram;

    // استخراج معلومات النظام
    get_system_info(os_name, os_version, username);

    // استخراج معلومات المعالج
    get_cpu_info(cpu_name, &num_cores);

    // استخراج معلومات الذاكرة
    get_memory_info(&total_ram);
    
    
	printf("Welcome With S.M.A Coding Channel...\n");
    // طباعة تقرير عن النظام
    printf("===== تقرير عن النظام =====\n");
    printf("اسم المستخدم: %s\n", username);
    printf("نظام التشغيل: %s %s\n", os_name, os_version);
    printf("المعالج: %s\n", cpu_name);
    printf("عدد النوى: %d\n", num_cores);
    printf("الذاكرة العشوائية: %d جيجابايت\n", total_ram);

    return 0;
}
