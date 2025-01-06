#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/version.h>
#include <linux/time.h>
#include <linux/timekeeping.h>

#define procfs_name "tsulab"
static struct proc_dir_entry *our_proc_file;

static unsigned long deviation_of_astronomical_year(void) {
    struct timespec64 ts;
    ktime_get_real_ts64(&ts);
    time64_t current_time_ts = ts.tv_sec;
    struct tm current_time_tm;
    time64_to_tm(current_time_ts, 0, &current_time_tm);

    int current_year = current_time_tm.tm_year + 1900;
    
    time64_t start_of_year = mktime64(current_year, 1, 1, 0, 0, 0);
    time64_t seconds_since_start_year = current_time_ts - start_of_year;

    if (current_year % 4 != 0) return seconds_since_start_year * (365.25 / 365.0 - 1.0) + (current_year % 4 - 1) * 6 * 60 * 60;
    else return 3 * 6 * 60 * 60 - (1.0 - 365.25 / 366) * seconds_since_start_year;
}

static ssize_t procfile_read(struct file *file_pointer, char __user *buffer, size_t buffer_length, loff_t *offset) {
    unsigned long deviation = deviation_of_astronomical_year();
    char str[128];
    int len;

    len = snprintf(str, sizeof(str), "Deviation of astronomical year in seconds: %lu\n", deviation);

    if (*offset > 0) {
        return 0;
    }

    if (copy_to_user(buffer, str, len)) {
        return -EFAULT;
    }

    *offset = len;

    pr_info("procfile read %s\n", file_pointer->f_path.dentry->d_name.name);
    return len;
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0)
static const struct proc_ops proc_file_fops = {
    .proc_read = procfile_read,
};
#else
static const struct file_operations proc_file_fops = {
    .read = procfile_read,
};
#endif

static int __init m_init(void) {
    our_proc_file = proc_create(procfs_name, 0644, NULL, &proc_file_fops); 
    if (NULL == our_proc_file) { 
        pr_alert("Error:Could not initialize /proc/%s\n", procfs_name); 
        return -ENOMEM; 
    } 
 
    pr_info("/proc/%s created\n", procfs_name); 
    return 0; 
}

static void __exit m_exit(void) { 
    proc_remove(our_proc_file);

    pr_info("/proc/%s removed\n", procfs_name); 
} 

module_init(m_init);
module_exit(m_exit);
MODULE_LICENSE("GPL");