#ifndef REGEX_COMMON_H
#define REGEX_COMMON_H
extern int regex_tracegen(int argc, char **argv);
extern int regex_default(int argc, char **argv);
extern void version();
extern unsigned int u_option;
extern void usage();

static struct conf {
	char *regex_file;
	char *in_file;
	char *out_file;
	char *dot_file;
	char *trace_file;
	bool i_mod;
	bool m_mod;
	bool verbose;
	bool debug;
	bool hfa;
   	char *trace_file_base;
	int num_seeds;
	bool depth_mode;
	bool size_mode;

} config;

extern void init_conf();
extern void print_conf();
extern int check_file(char *filename, char *mode);
extern int parse_arguments(int argc, char **argv);
#endif
