/***********************************************************/
/*** Return 1 if we're pretty sure this is a shadow pass ***/
/***********************************************************/

float is_shadow_pass()
{
    float fmt[3];
    string sc;
    float is = 0;
    option("Format",fmt);
    attribute("identifier:name",sc);
	/* square pic - can't be a regular camera */
    /* not a glimpse editor window? */
    if ((sc != "<unnamed>") &&
	((fmt[0] == fmt[1]) && (fmt[2] == 1.0)) )  {
	    is = 1;
    }
    return(is);
}

