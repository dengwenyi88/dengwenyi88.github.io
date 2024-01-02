package config

import (
	"context"
	"encoding/json"
	"fmt"
	"reflect"
	"strconv"
	"strings"
	"sync"
	"time"

	dapr "github.com/dapr/go-sdk/client"
)

var (
	DAPR_CONFIGURATION_STORE = "configstore"
	PGNOTIFY_CHANNEL         = "agile_config_trigger"
	syncOnce                 sync.Once
)

func (c *Config) Reload(app_id string, env string) {
	syncOnce.Do(func() {
		keys := c.keys(app_id, env)
		c.subcribeFromAgile(keys)
	})
}

func (c Config) keys(app_id string, env string) map[string]string {
	p := c
	t := reflect.TypeOf(p)
	k := t.Kind()
	t_app_id := fixedAppId(app_id)

	group := ""
	keys := make(map[string]string, 0)

	if k == reflect.Struct {
		value := reflect.ValueOf(p)
		numFiled := value.NumField()
		for i := 0; i < numFiled; i++ {
			name := t.Field(i).Name
			tag := t.Field(i).Tag.Get("agile")
			if tag == "" || tag == "-" {
				continue
			}

			val := fmt.Sprintf("%s.%s.%s.%s", t_app_id, env, group, tag)
			fmt.Printf("numFiled : %d,type : %s,value:%v,tag:%s,key:[%s]\n", i, value.Field(i).Kind(), value.Field(i), tag, val)
			keys[name] = val
		}
	}
	return keys
}

func (c *Config) setValues(keys map[string]string, items map[string]*dapr.ConfigurationItem) {
	p := c
	t := reflect.TypeOf(*p)
	k := t.Kind()

	if k == reflect.Struct {
		value := reflect.ValueOf(p)
		for name, key := range keys {
			// f := value.FieldByName(name)
			f := value.Elem().FieldByName(name)
			fmt.Printf("setValues kind:%s,name:%s,key:%s\n", f.Kind().String(), name, key)
			if !f.IsValid() {
				fmt.Printf("setValues key:%s not invalid of can't set,%v,%v\n", key, f.IsValid(), f.CanSet())
				continue
			}
			item, ok := items[key]
			if !ok {
				fmt.Printf("setValues key:%s not found\n", key)
				continue
			}
			k = f.Kind()
			switch k {
			case reflect.String:
				f.SetString(item.Value)
				fmt.Printf("setValues : key :%s,value: %s\n", key, item.Value)
			case reflect.Bool:
				ok := false
				if item.Value == "true" || item.Value == "1" {
					ok = true
				}
				f.SetBool(ok)
				fmt.Printf("setValues : key :%s,value: %v\n", key, ok)
			case reflect.Float64:
				val, err := strconv.ParseFloat(item.Value, 64)
				if err != nil {
					continue
				}
				f.SetFloat(val)
				fmt.Printf("setValues : key :%s,value: %v\n", key, val)
			default:
				fmt.Printf("setValues : key :%s,unknow type %s", key, k.String())
			}
		}

	}
}

func (c *Config) subcribeFromAgile(keys map[string]string) error {
	ctx := context.Background()
	client, err := dapr.NewClient()
	if err != nil {
		return err
	}

	err = client.Wait(ctx, 10*time.Second)
	if err != nil {
		return err
	}

	real_keys := make([]string, 0)
	for _, v := range keys {
		real_keys = append(real_keys, v)
	}

	items, err := client.GetConfigurationItems(ctx, DAPR_CONFIGURATION_STORE, real_keys)
	if err != nil {
		return err
	}
	c.setValues(keys, items)

	go doSubcribe(ctx, client, keys, c)
	return nil
}

func doSubcribe(ctx context.Context, client dapr.Client, keys map[string]string, c *Config) {
	handler := func(store string, items map[string]*dapr.ConfigurationItem) {
		jd, _ := json.Marshal(items)
		fmt.Printf("doSubcribe recv: %s\n", string(jd))
		c.setValues(keys, items)
	}

	opt := []dapr.ConfigurationOpt{
		// dapr.WithConfigurationMetadata("dapr-app-id", "test-secrets"),
		dapr.WithConfigurationMetadata("pgnotifychannel", PGNOTIFY_CHANNEL),
	}
	real_keys := make([]string, 0)
	for _, v := range keys {
		real_keys = append(real_keys, v)
	}

	// ctx, f := context.WithTimeout(ctx, 300*time.Second)
	// md := metadata.Pairs("dapr-app-id", "test-secrets")
	// ctx = metadata.NewOutgoingContext(ctx, md)
	// defer f()
	subscribeId, err := client.SubscribeConfigurationItems(ctx, DAPR_CONFIGURATION_STORE, real_keys, handler, opt...)
	if err != nil {
		panic(err)
	}
	fmt.Printf("doSubcribe wait...%s\n", subscribeId)
	<-ctx.Done()
	client.UnsubscribeConfigurationItems(ctx, DAPR_CONFIGURATION_STORE, subscribeId)
	fmt.Printf("doSubcribe done.%s\n", subscribeId)
}

func fixedAppId(app_id string) string {
	return strings.Replace(app_id, "-", "_", -1)
}
