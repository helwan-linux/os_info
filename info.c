/*
Welcome With Helwan O.S info demo version
https://github.com/helwan-linux/os_info
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
#include <Windows.h>
#include <stdint.h>
#elif __linux__
/*
لاسترداد معلومات النظام مثل اسم المضيف واسم نظام التشغيل 
*/
#include <sys/utsname.h>

/*
وظائف مثل getpid (الحصول على معرف الهوية). 
*/
#include <unistd.h>
/*
لاستخراج معلومات عن الهارد ديسك 
*/
#include <sys/stat.h>

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

//نوع الجهاز
void get_device_type(char *device_type) {
  #ifdef _WIN32
  // تعريف متغير لحفظ معلومات النظام
  SYSTEM_INFO sys_info;

  // استدعاء دالة GetSystemInfo للحصول على معلومات النظام
  GetSystemInfo(&sys_info);

  // استخراج نوع الجهاز
  DWORD dwProductType = sys_info.wProcessorArchitecture;

  // تحويل نوع الجهاز إلى سلسلة نصية
  switch (dwProductType) {
    case VER_NT_WORKSTATION:
      strcpy(device_type, "WORKSTATION");
      break;
    case VER_NT_SERVER:
      strcpy(device_type, "SERVER");
      break;
    default:
      strcpy(device_type, "UNKNOWN");
      break;
  }

  #elif __linux__
    // تعريف هيكل معلومات اسم نظام التشغيل
    struct utsname uname_data;

    // استدعاء دالة uname للحصول على معلومات اسم نظام التشغيل
    uname(&uname_data);

    // استخراج نوع الجهاز
    if (strstr(uname_data.machine, "x86_64")) {
      strcpy(device_type, "PC");
    } else if (strstr(uname_data.machine, "arm")) {
      strcpy(device_type, "ARM");
    } else {
      strcpy(device_type, "UNKNOWN");
    }

  #elif __APPLE__
    // تعريف متغيرات لحفظ معلومات النظام
    size_t size;
    char *buffer;

    // استخراج نوع الجهاز
    size = sizeof(device_type);
    sysctlbyname("hw.machine", device_type, &size, NULL, 0);

  #endif
}

//استخراج اسم الجهاز
void get_device_name(char *device_name) {
  #ifdef _WIN32
    // تعريف متغير لحفظ طول اسم الجهاز
    DWORD computer_name_len = UNLEN + 1;

    // استدعاء دالة GetComputerName للحصول على اسم الجهاز
    GetComputerName(device_name, &computer_name_len);

  #elif __linux__
    // استدعاء دالة gethostname للحصول على اسم الجهاز
    gethostname(device_name, UNLEN);

  #elif __APPLE__
    // استدعاء دالة SCDynamicStoreCopyComputerName للحصول على اسم الجهاز
    CFStringRef computer_name = SCDynamicStoreCopyComputerName(NULL, NULL);
    CFStringGetCString(computer_name, device_name, sizeof(device_name), kCFStringEncodingUTF8);
    CFRelease(computer_name);

  #endif
}

//معلومات القرص الصلب
void get_disk_info(uint64_t *total_space, uint64_t *used_space, uint64_t *free_space) {
  #ifdef _WIN32
    // تعريف متغيرات لحفظ معلومات القرص
    ULARGE_INTEGER totalSpace, freeSpace;

    // استدعاء دالة GetDiskFreeSpaceExW للحصول على معلومات القرص
    GetDiskFreeSpaceExW(L"C:\\", &freeSpace, &totalSpace, NULL);

    // استخراج المساحة الكلية
    *total_space = totalSpace.QuadPart;

    // استخراج المساحة المستخدمة
    *used_space = totalSpace.QuadPart - freeSpace.QuadPart;

    // استخراج المساحة الفارغة
    *free_space = freeSpace.QuadPart;

  #elif __linux__
    // تعريف هيكل معلومات القرص
    struct statvfs stat;

    // استدعاء دالة statvfs للحصول على معلومات القرص
    statvfs("/", &stat);

    // استخراج المساحة الكلية
    *total_space = stat.f_blocks * stat.f_bsize;

    // استخراج المساحة المستخدمة
    *used_space = stat.f_blocks_used * stat.f_bsize;

    // استخراج المساحة الفارغة
    *free_space = stat.f_bavail * stat.f_bsize;

  #elif __APPLE__
    // تعريف متغيرات لحفظ معلومات القرص
    vm_size_t page_size;
    mach_port_t mach_port;
    vm_statistics_data_t vm_stats;

    // استخراج حجم صفحة الذاكرة
    host_page_size(mach_host_self(), &page_size);

    // استخراج معلومات الذاكرة
    mach_port = mach_host_self();
    if (host_statistics(mach_port, HOST_VM_INFO, (host_info_t)&vm_stats, &info_count) == KERN_SUCCESS) {
      *total_space = vm_stats.active_memory / page_size;
      *used_space = vm_stats.wire_memory / page_size;
      *free_space = vm_stats.free_memory / page_size;
    }

  #endif
}



void get_system_info(char *os_name, char *os_version, char *username) {
  // تعريف هيكل معلومات إصدار نظام التشغيل
  #ifdef _WIN32
  OSVERSIONINFOEX os_info;
  #elif __APPLE__
  size_t size;
  char *buffer;
  #endif

  // استدعاء دالة GetVersionEx أو sysctlbyname للحصول على معلومات إصدار نظام التشغيل
  #ifdef _WIN32
  GetVersionEx((OSVERSIONINFO *)&os_info);
  strcpy(os_name, os_info.szCSDVersion);
  sprintf(os_version, "%ld.%ld.%ld", os_info.dwMajorVersion, os_info.dwMinorVersion, os_info.dwBuildNumber);
  #elif __linux__
  struct utsname uname_data;
  uname(&uname_data);
  strcpy(os_name, uname_data.sysname);
  strcpy(os_version, uname_data.release);
  #elif __APPLE__
  size = sizeof(os_name);
  sysctlbyname("kern.ostype", os_name, &size, NULL, 0);
  size = sizeof(os_version);
  sysctlbyname("kern.osrelease", os_version, &size, NULL, 0);
  #endif

  // تعريف متغير لحفظ طول اسم المستخدم
  #ifdef _WIN32
  DWORD username_len = UNLEN + 1;
  #endif

  // استدعاء دالة GetUserName أو getlogin_r للحصول على اسم المستخدم
  #ifdef _WIN32
  GetUserName(username, &username_len);
  #elif __linux__
  getlogin_r(username, UNLEN + 1);
  #elif __APPLE__
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
  // تعريف متغيرات لحفظ معلومات النظام
  char os_name[UNLEN], os_version[UNLEN], username[UNLEN], device_type[UNLEN], device_name[UNLEN];
  uint64_t total_space, used_space, free_space;

  // استخراج معلومات النظام
  get_system_info(os_name, os_version, username);
  get_device_type(device_type);
  get_device_name(device_name);
  get_disk_info(&total_space, &used_space, &free_space);

  // طباعة معلومات النظام
  printf("**معلومات النظام**\n");
  printf("اسم نظام التشغيل: %s\n", os_name);
  printf("إصدار نظام التشغيل: %s\n", os_version);
  printf("اسم المستخدم: %s\n", username);
  printf("نوع الجهاز: %s\n", device_type);
  printf("اسم الجهاز: %s\n", device_name);
  printf("**معلومات القرص الصلب**\n");
  printf("المساحة الكلية: %llu جيجابايت\n", total_space);
  printf("المساحة المستخدمة: %llu جيجابايت\n", used_space);
  printf("المساحة الفارغة: %llu جيجابايت\n", free_space);

  return 0;
}

