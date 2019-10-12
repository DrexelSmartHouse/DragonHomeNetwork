def get_conf(conf_file):
    """ 
    This function takes the config file as an argument 
    and returns a dictionary of the value contained in
    the file.
    """

    conf_dic = dict()

    for line in conf_file:

        # remove the comment from the line
        if '#' in line:
            line = line[:line.index('#')]

        # build the dictionary from the ':' seperated pairs
        if ':' in line:
            line = line.split(':')
            key = line[0].strip()
            value = line[1].strip()

            # convert to int if possible
            try:
                value = int(value)
            except:
                pass

            conf_dic[key] = value

    return conf_dic


# small test prints the dictionary
if __name__ == '__main__':

    filename = 'dshPython.conf'
    file = open(filename, "r")

    conf = get_conf(file)

    print(conf)

    # for key, value in conf.iteritems():
    #    print key + ': ' + str(value)
